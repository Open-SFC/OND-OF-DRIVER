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

/* File  :      dprm.c
 * Author:      Rajesh Madabushi <rajesh.madabushi@freescale.com>
 * Description: Definitions of interface between datapath and transport layer
 */

/*INCLUDE_START  ********************************************************/
#include "controller.h"
#include "cntrl_queue.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "of_utilities.h"
#include "of_msgapi.h"
#include "of_multipart.h"
#include "cntlr_event.h"
#include "dprm.h"
#include "dprmldef.h"
/*INCLUDE_END  **********************************************************/

/*DEFINE START  *********************************************************/
//#define of_debug printf
#define OF_SIF_MAIN_CONN_ID  (0)
/*DEFINE_END    ********************************************************/

extern void *of_app_hook_mempool_g;
extern of_list_t datapath_notifications[];
extern void 
of_async_msg_domain_init(struct dprm_domain_entry *p_domain);
extern int32_t get_datapath_port_byhandle(uint64_t datapath_handle,uint64_t port_handle,struct dprm_port_entry** port_info_p_p);

inline dprm_dp_channel_entry_t*
of_get_dp_channel(uint64_t  dp_handle, uint32_t  auxiliary_id);
//static void of_msg_get_mempool_entries(uint32_t *max_entries_p, uint32_t *max_static_entries_p);
inline int32_t dprm_dp_table_init(uint8_t   no_of_tables,
                                  void     **tables);
inline void of_dp_table_de_init(void* tables);

int32_t
cntrl_add_table_to_dp(struct dprm_datapath_entry *datapath,
                 struct dprm_oftable_entry  *domain_table_entry);

inline int32_t
cntlr_get_table_info(struct dprm_datapath_entry *datapath,
                     uint8_t                     table_id,
                     dprm_dp_table_entry_t **p_p_table_entry);

void of_app_info_free_rcu(struct rcu_head *node);

/*TBD, cert_info_ requirement and if required data_strucuture is TBD*/

void
of_free_msg_wait_on_barrier_queue_rcu(struct rcu_head *node)
{
    struct barrier_state_msg_queue_node *msg_node = (struct barrier_state_msg_queue_node*)((uint8_t*)(node) - sizeof(of_list_node_t));
    
#if 0
    if(msg_node->msg != NULL)
      msg_node->msg->desc.free_cbk(msg);
#endif

    of_mem_free(msg_node);
}

int32_t
cntlr_datapath_init(uint64_t                    datapath_id,
                    struct dprm_datapath_entry* datapath,
                    struct dprm_domain_entry*   domain_p)
{
    OF_STORE_DATAPATH_ID(datapath->datapath_id,datapath_id);

    datapath->controller_role = OF_CNTLR_ROLE_DEFAULT;

    OF_LIST_INIT(datapath->table_list,NULL);
    OF_LIST_INIT(datapath->group_table, NULL);
    OF_LIST_INIT(datapath->meter_table, NULL);
    OF_LIST_INIT(datapath->flow_table, NULL);

    OF_LIST_INIT(datapath->msg_queue_in_barrier_state,of_free_msg_wait_on_barrier_queue_rcu);
    datapath->barrier_state = 0;

    datapath->is_main_conn_present = FALSE;

    datapath->curr_no_of_aux_chanels = 0;
    CNTLR_LOCK_INIT(datapath->aux_ch_list_lock);
    datapath->aux_channels = NULL;
    datapath->aux_chn_tail = NULL;
    datapath->domain = domain_p;

#if 0
    for(tableno = 0; tableno < domain_p->number_of_tables;tableno++)
    {
      if(cntlr_add_table_to_dp(datapath, (domain_p->table_p + tableno) ) == OF_FAILURE)
      {
         for(kk = 0 ; kk < tableno; kk++)
         {      
            /*TBD remove table from list*/
         }
         return OF_FAILURE;
      }
    }
#endif

#if 0
     if(pCertInfo)
     {
        /* TBD Store and Process DP Certificate details*/
     }

#endif
    cntlr_no_of_dps_g++;

   return OF_SUCCESS;
}

#if 0

int32_t
cntlr_datapath_deinit(uint64_t  datapath_id_p)
{
#if 0
   struct dprm_datapath_entry  *datapath;
   of_DPProfile_t    *pProfile;
   of_data_path_id_t    datapath_id;
   uint32_t           ulHash;
   uint8_t            bStatus;

   CNTLR_RCU_READ_LOCK_TAKE();

   OF_SELECT_DP_PROFILE(pProfile,pProfile_p);
   psp_assert(pProfile);

   OF_STORE_DATAPATH_ID(datapath_id,datapath_id)
   datapath = of_GetDPIface(&datapath_id);
   if(datapath == NULL)
   {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Get DP Interface details from Physical SW failed\r\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
   }

   of_RemoveDPFromProfileList(pProfile,&datapath_id);

   of_RemoveDPFromPhySWList(&datapath->pDPInfo->datapath_id);

   OF_DATAPATH_TABLE_HASH(datapath_id.ul[0], datapath_id.ul[1], pProfile->uiID,ulHash);
   CNTLR_MCHASH_DELETE_NODE(of_pDPTable,ulHash,datapath,bStatus);
   if(bStatus == FALSE)
   {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Failed to delete Datapath ID(ID = %llu) node\n",
                            __FUNCTION__,__LINE__,datapath->pDPInfo->datapath_id.val);
      return OF_FAILURE;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
#endif

   return OF_SUCCESS;
}
#endif

int32_t
cntlr_domain_init(struct dprm_domain_entry *p_domain)
{
   uint32_t                 msgno;
   uint32_t                 eventno;

   /*Initialize the lock used to set internal domain variables*/
   CNTLR_LOCK_INIT(p_domain->lock);

   /*Reset default memory primitive details*/
   memset(&p_domain->default_msg_mem_primitive,0,sizeof(struct of_async_msg_mem_primitive));

   /*Reset async msg primitives*/
  for(msgno = 1; msgno <= OF_MAX_NUMBER_ASYNC_MSGS; msgno++)
      memset(&p_domain->msg_type_mem_primitive[msgno],0,sizeof(struct of_async_msg_mem_primitive));
 
  for(msgno = 1; msgno <= OF_MAX_NUMBER_SYM_MSGS; msgno++)
      memset(&p_domain->symmetric_msg_handle[msgno],0,sizeof(struct symmetric_msg_info));

  for(eventno = 1; eventno <= CNTLR_MAX_EVENTS; eventno++)
  {
     memset(&p_domain->app_event_info[eventno],0,sizeof(struct domain_app_event_info));
     OF_LIST_INIT(p_domain->app_event_info[eventno].app_list,of_app_info_free_rcu);
  }

  return OF_SUCCESS;
}

#if 0
/*Call MUST use RCU Lock and RCU UNLOCK*/
int32_t
cntlr_get_domain_table_info(struct dprm_domain_entry *domain,
                            uint8_t                   table_id,
                            struct dprm_table_entry  **p_p_table)
{
   struct dprm_table_entry* table;
   uint32_t                 tableno;

   for(tableno = 0; tableno < domain->number_of_tables_id_based;tableno++)
   {

       if(domain->table_p[tableno].table_id  == table_id)
       {
          *p_p_table = (domain->table_p + tableno);
          return OF_SUCCESS;
       }
   }

   return OF_FAILURE;
}
#endif


inline int32_t
dprm_dp_table_init(uint8_t   no_of_tables,
                   void     **tables_p)
{
    dprm_dp_table_entry_t *tables;

    tables = (dprm_dp_table_entry_t*) of_mem_calloc(no_of_tables,
                                                    sizeof(dprm_dp_table_entry_t));
    if(tables == NULL)
    {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Memory Allocation Failed - Datapath Tables \n",__FUNCTION__,__LINE__);
        return OF_FAILURE;
    }

    *tables_p = (void*)tables;

    return OF_SUCCESS;
}

inline void
of_dp_table_de_init(void* tables)
{
    of_mem_free((dprm_dp_table_entry_t*)tables);
}


inline void
of_channel_free_rcu(struct rcu_head *node)
{
  dprm_dp_channel_entry_t *dp_channel = (dprm_dp_channel_entry_t*)node;

  of_mem_free(dp_channel);
}

int32_t
of_remove_channel_from_dp(uint64_t channel_dp_handle,
                          uint32_t  auxiliary_id)
{
  struct dprm_datapath_entry  *datapath;
  dprm_dp_channel_entry_t     *dp_channel;
  int32_t                      status = OF_SUCCESS;
  int32_t                      retval = OF_SUCCESS;
  int8_t status_b;

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
     dp_channel = of_get_dp_channel(channel_dp_handle,auxiliary_id);
     if(dp_channel == NULL)
     {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Channel(%#x) not configured in datapath \r\n", __FUNCTION__,__LINE__,auxiliary_id);
        status = OF_FAILURE;
        break;
     }

     /** Get Datapath information by giving datapath handle */
     retval = get_datapath_byhandle(dp_channel->dp_handle, &datapath);
     if(retval  == OF_FAILURE)
     {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in getting datapath datails\n",__FUNCTION__,__LINE__);
         status = OF_FAILURE;
         break;
     }

     /*TBD del Transport FD from corresponding thread epoll context, 
      * how to get thread ID, may be need to use it the channel*/

    if(auxiliary_id != OF_TRNSPRT_MAIN_CONN_ID)
    {
        CNTLR_LOCK_TAKE(datapath->aux_ch_list_lock);
        CNTLR_RCU_ASSIGN_POINTER(dp_channel->prev->next,dp_channel->next);
        CNTLR_RCU_ASSIGN_POINTER(dp_channel->next->prev,dp_channel->prev);
        datapath->curr_no_of_aux_chanels--;
        if(datapath->curr_no_of_aux_chanels == 0)
           CNTLR_RCU_ASSIGN_POINTER(datapath->aux_channels,NULL);
        else
           if(datapath->aux_channels == dp_channel)
              CNTLR_RCU_ASSIGN_POINTER(datapath->aux_channels,datapath->aux_channels->next);
        CNTLR_CALL_RCU((struct rcu_head *)dp_channel, of_channel_free_rcu);
        CNTLR_LOCK_RELEASE(datapath->aux_ch_list_lock);
    }
    else
    {
       if(dp_channel->datapath)
       {
         dp_channel->datapath->is_main_conn_present = FALSE; //Deepthi
         of_dp_table_de_init((dprm_dp_table_entry_t *)dp_channel->datapath->tables); //Deepthi
         dp_channel->datapath->no_of_buffers        = 0;
         dp_channel->datapath->no_of_tables         = 0;
         dp_channel->datapath->capabilities         = 0;
         memset(&dp_channel->datapath->main_channel, 0, sizeof(dprm_dp_channel_entry_t)); //Deepthi

         dp_channel->dp_handle          = 0; 
         dp_channel->datapath           = NULL; 
       }
       if(dp_channel->conn_table)
       {
	       OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"deletin channel from connection table");
	       dp_channel->conn_safe_ref.index = 0;
	       dp_channel->conn_safe_ref.magic = 0;
	       dp_channel->conn_table = NULL;
       }
       /*memset(dp_channel,0, sizeof(dprm_dp_channel_entry_t));
       datapath->is_main_conn_present = FALSE; */ //Deepthi
       /*TBD removing of auxiliary channels*/
    }

  } while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  return status;
}
/*Caller MUST use rcu_read_lock() and rcu_read_unlock()*/
inline dprm_dp_channel_entry_t*
of_get_dp_channel(uint64_t  dp_handle,
                  uint32_t  auxiliary_id)
{
   dprm_dp_channel_entry_t     *curr_dp_chn_copy;
   dprm_dp_channel_entry_t     *curr_dp_channel;
   struct dprm_datapath_entry  *datapath;
   int32_t                      retval = OF_SUCCESS;

   /** Get Datapath information by giving datapath handle */
   retval = get_datapath_byhandle(dp_handle, &datapath);
   if(retval  == OF_FAILURE)
   {
       OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in getting datapath datails\n",__FUNCTION__,__LINE__);
       return NULL;
   }
   if(auxiliary_id == OF_TRNSPRT_MAIN_CONN_ID )
   {
      /*Main channel is static memory so RCU operation not required*/
      return &datapath->main_channel; 
   }
   else
   {
      curr_dp_channel = datapath->aux_channels;
      do
      {
         if(curr_dp_channel->auxiliary_id == auxiliary_id)
         {
            CNTLR_RCU_DE_REFERENCE(curr_dp_channel,curr_dp_chn_copy);
            return curr_dp_chn_copy;
         }
         curr_dp_channel = curr_dp_channel->next;
      }
      while(curr_dp_channel != datapath->aux_channels);
   }
   return NULL;
}
/*Caller MUST use rcu_read_lock() and rcu_read_unlock()*/
inline dprm_dp_channel_entry_t*
of_get_first_aux_channel(struct dprm_datapath_entry *datapath)
{
   dprm_dp_channel_entry_t   *dp_channel_copy;

   CNTLR_RCU_DE_REFERENCE(datapath->aux_channels,dp_channel_copy);
   return dp_channel_copy;
}
/*Caller MUST use rcu_read_lock() and rcu_read_unlock()*/
inline dprm_dp_channel_entry_t*
of_get_next_aux_channel(struct dprm_datapath_entry   *datapath,
                     dprm_dp_channel_entry_t *dp_channel)
{
   dprm_dp_channel_entry_t   *dp_channel_copy;

   if( dp_channel->next == (datapath->aux_channels) )
      return NULL; 

   CNTLR_RCU_DE_REFERENCE(dp_channel->next,dp_channel_copy);
   return dp_channel_copy;
}



void
of_app_info_free_rcu(struct rcu_head *node)
{
    struct of_app_hook_info *app_info = (struct of_app_hook_info*)((uint8_t*)(node) - sizeof(of_list_node_t));

    mempool_release_mem_block (of_app_hook_mempool_g,(unsigned char*)app_info,app_info->is_heap);
}

/***********************************************************************/
	int32_t
of_add_new_role_to_dp(uint64_t datapath_handle,  uint32_t controller_role)
{

	int32_t retval=OF_SUCCESS;
	struct dprm_datapath_entry     *datapath = NULL;

	/** Get Datapath information by giving datapath handle */
	retval = get_datapath_byhandle(datapath_handle, &datapath);
	if(retval  == OF_FAILURE)
	{
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in getting datapath datails\n",__FUNCTION__,__LINE__);
		return retval ;
	}
	datapath->controller_role = controller_role;
	return retval; 
}

/************************************************************************/
int32_t
of_add_port_to_dp(uint64_t datapath_handle,
                  struct ofl_port_desc_info   *port_desc)
{
    struct dprm_port_info   port_info;
    struct dprm_port_entry *dp_port_entry;
   
    /* notification data  to crm module */
    struct dprm_datapath_notification_data notification_data={};
    uint64_t                port_handle;
    int32_t  retval = OF_SUCCESS;

   struct   dprm_notification_app_hook_info  *app_entry_p;
   uchar8_t lstoffset;
   lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;


           OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Entered");


    retval = dprm_get_port_handle(datapath_handle,port_desc->name,&port_handle);
    if(retval == DPRM_SUCCESS)
    {
        retval = get_datapath_port_byhandle(datapath_handle,port_handle,&dp_port_entry);
        if(retval != DPRM_SUCCESS)
        {
           OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in getting port  %s err=%d \r\n",
                                           __FUNCTION__,__LINE__,port_desc->name,retval);
           return OF_FAILURE;
        
        }
       

        /* copy the port id*/ 
        dp_port_entry->port_id = port_desc->port_no;
        memcpy(&dp_port_entry->port_info,port_desc,sizeof(struct ofl_port_desc_info));



        /** Trigger DPRM_DATAPATH_PORT_DESC_UPDATE event .
	    crm module registers with all notitications event. 
	    crm module updates its port db with this event **/ 
	strncpy(notification_data.port_name, port_desc->name, OFP_MAX_PORT_NAME_LEN);
	notification_data.port_id     = dp_port_entry->port_id ;
	notification_data.port_handle = port_handle;


	OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_ALL_NOTIFICATIONS],
		     app_entry_p,
		     struct dprm_notification_app_hook_info*,
		     lstoffset 
		    )
	{
		((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_PORT_DESC_UPDATE,
		datapath_handle,
		notification_data,
		app_entry_p->cbk_arg1,
		app_entry_p->cbk_arg2);
  	}
        OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_PORT_DESC_UPDATE],
                     app_entry_p,
                     struct dprm_notification_app_hook_info*,
                     lstoffset
                    )
        {
                ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_PORT_DESC_UPDATE,
                datapath_handle,
                notification_data,
                app_entry_p->cbk_arg1,
                app_entry_p->cbk_arg2);
        }
    }
    else
    {
       strncpy(port_info.port_name,port_desc->name,OFP_MAX_PORT_NAME_LEN);
       port_info.port_id = port_desc->port_no;
       retval = dprm_add_port_to_datapath(datapath_handle,&port_info,&port_handle);
       if(retval != DPRM_SUCCESS)
       {
           OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in adding port %s to datapath,err=%d \r\n",
                                           __FUNCTION__,__LINE__,port_desc->name,retval);
        
           return OF_FAILURE;
       }

       retval = get_datapath_port_byhandle(datapath_handle,port_handle,&dp_port_entry);
       if(retval != DPRM_SUCCESS)
       {
           OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in getting port %s from datapath,err=%d \r\n",
                                           __FUNCTION__,__LINE__,port_desc->name,retval);
        
           return OF_FAILURE;
       }
      
       memcpy(&dp_port_entry->port_info,port_desc,sizeof(struct ofl_port_desc_info));

    }
    return OF_SUCCESS;
}

#if 0

int32_t
cntlr_add_table_to_dp(struct dprm_datapath_entry *datapath,
                      struct dprm_oftable_entry  *domain_table_entry)
{
    dprm_dp_table_entry_t *curr_table_entry = NULL;
    dprm_dp_table_entry_t *prev_table_entry = NULL;
    dprm_dp_table_entry_t *p_table_entry;

    CNTLR_RCU_READ_LOCK_TAKE();

    p_table_entry = calloc(1, sizeof(dprm_dp_table_entry_t));
    if(p_table_entry == NULL)
    {
       OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d memory allocation failure  dp table entry \r\n", __FUNCTION__,__LINE__);
        
       return OF_FAILURE;
    }
      
    p_table_entry->table_id = domain_table_entry->table_id; 
    strncpy(p_table_entry->table_name,domain_table_entry->table_name,OFP_MAX_TABLE_NAME_LEN);
    p_table_entry->datapath = datapath;

    OF_LIST_SCAN(datapath->table_list,curr_table_entry,dprm_dp_table_entry_t*)
    {
        if(p_table_entry->table_id <= curr_table_entry->table_id)
        {
          OF_LIST_INSERT_NODE(datapath->table_list,
                              prev_table_entry,
                              p_table_entry,
                              curr_table_entry);
           return OF_SUCCESS;
        }
        prev_table_entry = curr_table_entry;
    }

    OF_APPEND_NODE_TO_LIST(datapath->table_list,p_table_entry);

    CNTLR_RCU_READ_LOCK_RELEASE();
    return OF_SUCCESS;
}

inline int32_t
cntlr_get_table_info(struct dprm_datapath_entry *datapath,
                     uint8_t                     table_id,
                     dprm_dp_table_entry_t **p_p_table_entry)
{
    dprm_dp_table_entry_t *p_table_entry;

    OF_LIST_SCAN(datapath->table_list,p_table_entry,dprm_dp_table_entry_t*)
    {
        if(p_table_entry->table_id == table_id)
        {
           CNTLR_RCU_DE_REFERENCE(p_table_entry,*p_p_table_entry);
           return OF_SUCCESS;
        }
    }
    return OF_FAILURE;
}
#endif

#if 0

/* Caller must use rce read lock and unlock*/
int32_t
of_GetDPPortInfo(of_DapathInfo_t *pDPInfo,
                 uint32_t         PortNo,
                 of_PortEntry_t **ppPort)
{
    of_PortEntry_t *pPortEntry;

    OF_LIST_SCAN(pDPInfo->PortList,pPortEntry,of_PortEntry_t*)
    {
        if(PortNo == pPortEntry->Port.port_no)
        {
           CNTLR_RCU_DE_REFERENCE(pPortEntry,*ppPort);
           return OF_SUCCESS;
        }
    }

    return OF_FAILURE;
}

void
of_PortFreeRcu(struct rcu_head *pNode_p)
{
    of_PortEntry_t *pAppEntry = FieldContainerPtr(pNode_p,of_PortEntry_t,rcu_head);

    of_mem_free(pAppEntry);
}

int32_t
of_RemovePortFromDP(of_DapathInfo_t *pDPInfo,
                    struct ofp_port *pPort)
{
    of_PortEntry_t *pPortEntry;
    of_PortEntry_t *pPortPrevEntry = NULL;

    CNTLR_RCU_READ_LOCK_TAKE();
    OF_LIST_SCAN(pDPInfo->PortList,pPortEntry,of_PortEntry_t*)
    {
        if(pPort->port_no == pPortEntry->Port.port_no)
        {
           OF_REMOVE_NODE_FROM_LIST(pDPInfo->PortList,pPortEntry,pPortPrevEntry);
       
           of_InformAllAppsabtPortStatusChange(pDPInfo->AppList, OF_PORT_DEL_EVENT,&pPortEntry->Port);
           return OF_SUCCESS;
        }
        pPortPrevEntry = pPortEntry;
    }
    CNTLR_RCU_READ_LOCK_RELEASE();

    return OF_FAILURE;
}

int32_t
of_ModifyDPPort(of_DapathInfo_t *pDPInfo,
                struct ofp_port *pPort)
{
    of_PortEntry_t *pPortEntry;

    CNTLR_RCU_READ_LOCK_TAKE();
    OF_LIST_SCAN(pDPInfo->PortList,pPortEntry,of_PortEntry_t*)
    {
        if(pPort->port_no == pPortEntry->Port.port_no)
        {
           CNTLR_LOCK_TAKE(pPortEntry->Lock);
           pPortEntry->Port.port_no = ntohl(pPort->port_no);
           pPortEntry->Port.config = ntohl(pPort->config);
           pPortEntry->Port.state = ntohl(pPort->state);
           pPortEntry->Port.curr = ntohl(pPort->curr);
           pPortEntry->Port.advertised = ntohl(pPort->advertised);
           pPortEntry->Port.supported = ntohl(pPort->supported);
           pPortEntry->Port.peer = ntohl(pPort->peer);
           pPortEntry->Port.curr_speed = ntohl(pPort->curr_speed);
           pPortEntry->Port.max_speed = ntohl(pPort->max_speed);

           memcpy(pPortEntry->Port.hw_addr,pPort->hw_addr,OFP_ETH_ALEN);

           strncpy(pPortEntry->Port.name,
                   pPort->name,
                   OFP_MAX_PORT_NAME_LEN);
          /*Remember the Port status change message in open flow format, used for communication
           * with DP */
           memcpy(&pPortEntry->ofp_Port,pPort,sizeof(struct ofp_port));

           CNTLR_LOCK_RELEASE(pPortEntry->Lock);
           of_InformAllAppsabtPortStatusChange(pDPInfo->AppList, OF_PORT_MOD_EVENT,&pPortEntry->Port);
           return OF_SUCCESS;
        }
    }
    CNTLR_RCU_READ_LOCK_RELEASE();

    return OF_FAILURE;
}
#endif
