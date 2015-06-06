
#ifdef  CNTRL_CBENCH_SUPPORT

#include "controller.h"
#include "of_utilities.h"
#include "of_multipart.h"
#include "of_msgapi.h"
#include "cntrl_queue.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_event.h"
#include "dprmldef.h"
#include "dprm.h"
#include "pktmbuf.h"

#define CBENCH_APP_TABLE_ID 0
#define CBENCH_APP_PKT_IN_PRIORITY 5
#define CBENCH_APP_NO_OF_TABLES 1
#define NO_OF_CBENCH_SWIITCHES  8

static void cbench_domain_table_event_cbk(uint32_t notification_type,
    uint64_t domain_handle,
    struct   dprm_domain_notification_data notification_data,
    void     *callback_arg1,
    void     *callback_arg2);
extern int32_t cntlr_app_init(uint64_t  domain_handle);

int32_t
cbench_app(uint64_t              controller_handle,
           uint64_t               domain_handle,
           uint64_t               datapath_handle,
           struct of_msg         *pkt_in_msg,
           struct ofl_packet_in  *pkt_in,
           void                  *cbk_arg1,
           void                  *cbk_arg2)
{
   uint16_t       msg_len;
   struct of_msg *msg;
   uint16_t      match_fd_len;
   uint8_t       *match_start_loc=NULL;
   uint16_t      flags;
   int32_t       retval = OF_SUCCESS;
   int32_t       status = OF_SUCCESS;
   void *conn_info_p;

 if(pkt_in_msg != NULL)
      of_get_pkt_mbuf_by_of_msg_and_free(pkt_in_msg);

   msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+ 8+ 8);
   do
   {
	   msg = ofu_alloc_message(OFPT_FLOW_MOD,
			   msg_len);
	   if(msg == NULL)
	   {
		   OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
		   status = OF_FAILURE;
		   break;
	   }

	   flags = 0;
	   flags |= OFPFF_RESET_COUNTS;

	   retval = of_create_add_flow_entry_msg(msg,
			   datapath_handle,
			   0, /*Table Id 0*/
			   0, /* Low priority*/
                           OFPFC_ADD,
			   0, /*coockie*/
			   0,/*Coockie amsk*/
			   OFP_NO_BUFFER,
			   flags,
			   0, /*No Hard timeout*/
			   0, /* No Idle timeout*/
			   &conn_info_p
			   );
	   if(retval != OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS)
	   {
		   OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d  Error in adding miss entry  = %d\r\n",
				   __FUNCTION__,__LINE__,retval);
		   status = OF_FAILURE;
		   break;
	   }
	   /*TBD, if we are not pusshing any match fields,this might be not rquired*/ 
	   ofu_start_setting_match_field_values(msg);
	   ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);

	   retval=of_cntrlr_send_request_message(
			   msg,
			   datapath_handle,
			   conn_info_p,
			   NULL,
			   NULL);
	   if (retval != OF_SUCCESS)
	   {
		   OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR," Error adding flow entry in route table = %d",retval);
		   status = OF_FAILURE;
	   }


   }
   while(0);

   return OF_ASYNC_MSG_CONSUMED;
}

int32_t cbench_config_resources()
{
  struct dprm_switch_info                         switch_1;
  struct dprm_datapath_general_info               datapath_info_1;
  //struct dprm_datapath_general_info               datapath_info_2;
  //uint32_t                                        domain_info_size;
  uint64_t                                        switch_handle_1 = 0;
  uint64_t                                        domain_handle_1     = 0;
  uint64_t                                        dp_handle_1         = 0;
  //uint64_t                                        dp_handle_2         = 0;
  uint32_t                                        retval = OF_SUCCESS;
  uint32_t                                        status = OF_SUCCESS;

  do
  {

    struct dprm_table_info table_info;
    struct dprm_oftable_entry *p_oftable_entry;
    uint64_t  table_handle;

  
    if (dprm_register_forwarding_domain_notifications(DPRM_DOMAIN_ALL_NOTIFICATIONS,
          cbench_domain_table_event_cbk,
          NULL,NULL) != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:Registering to cbench domain failed....\r\n",__FUNCTION__);
      break;
    }

  }
  while(0);

  if(status == OF_FAILURE)
  {
    /*TBD release of all resources*/
    return OF_FAILURE;
  }

  return OF_SUCCESS;
}

int32_t cbench_app_init()
{
  struct dprm_distributed_forwarding_domain_info  domain_info_1;
  struct dprm_table_info table_info;
  uint32_t                                        status = OF_SUCCESS;

  /* configuring resoures required for cbench application */
  cbench_config_resources();
  return OF_SUCCESS;
}


static void cbench_domain_table_event_cbk(uint32_t notification_type,
    uint64_t domain_handle,
    struct   dprm_domain_notification_data notification_data,
    void     *callback_arg1,
    void     *callback_arg2)
{
  struct  dprm_domain_entry* domain_entry_p;
  int32_t ret_val;

  OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_DEBUG,"%s:%d domain table add event received\r\n",__FUNCTION__,__LINE__);

  if(strcmp(notification_data.ttp_name, "CBENCH_TTP"))
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"notification is  not for cbench_ttp.");
    return;
  }

  ret_val = get_domain_byhandle(domain_handle,&domain_entry_p);
  if(ret_val != DPRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d get_domain_byhandle failed",__FUNCTION__,__LINE__);
    return;
  }


  if(of_register_asynchronous_message_hook(domain_handle,
			  CBENCH_APP_TABLE_ID,
			  OF_ASYNC_MSG_PACKET_IN_EVENT,
			  CBENCH_APP_PKT_IN_PRIORITY,
			  (void*)cbench_app,NULL,NULL) == OF_FAILURE)
  {
	  OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Register Forwarding app to receive pkt in failed\r\n",__FUNCTION__,__LINE__);
	  return;
  }
  return;
}
#endif
