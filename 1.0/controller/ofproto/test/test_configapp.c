
#if 0
#include "controller.h"
#include "of_utilities.h"
#include "of_msgapi.h"
#include "cntrl_queue.h"
#include "of_msgpool.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_event.h"
#include "dprmldef.h"

//#include "of_msgapi.h"
#define of_debug printf
#define FLOW_APP_NO_OF_TABLES  3

#define OF_DEFAULT_SW_MISS_SEND_LEN  128

int32_t config_resources();



#if 0

void
of_handle_echo_request_msg(uint64_t controller_handle,
                           uint64_t domain_handle,
                           uint64_t datapath_handle,
                           struct of_msg  *msg,
                           struct ofi_echo_req_recv_msg *echo_req,
                           void *cbk_arg1,
                           void *cbk_arg2)
{
    struct ofp_header *msg_hdr;
    struct dprm_datapath_entry *datapath;
    cntlr_conn_node_saferef_t conn_safe_ref;
    cntlr_conn_table_t        *conn_table;
    int32_t                    retval = OF_SUCCESS;

    printf("%s %d %d\r\n",__FUNCTION__,__LINE__,cntlr_thread_index);
   /*Sending back same openflow message, not allocating new message buffer*/
    msg_hdr = (struct ofp_header*)(msg->desc.start_of_data);
    msg_hdr->type = OFPT_ECHO_REPLY;
    msg_hdr->length =  ntohs(msg_hdr->length);
    msg_hdr->xid = ntohl(msg_hdr->xid);

     printf("%s:%d dp_handle in channel %llx\r\n",__FUNCTION__,__LINE__,datapath_handle);
    retval = get_datapath_byhandle(datapath_handle, &datapath);
    if(retval  != DPRM_SUCCESS)
    {
       OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Error in getting datapath info, Err = %d\n",
                                                 __FUNCTION__,__LINE__,retval);
       return;
    }

    CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
    //retval = cntlr_send_msg_to_dp(msg,datapath_handle,conn_table,&conn_safe_ref,NULL,NULL,NULL);
    retval = cntlr_transprt_send_to_dp_locally(msg,conn_table,&conn_safe_ref);
    if(retval  == OF_FAILURE)
    {
       OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Error in sending the message\n",__FUNCTION__,__LINE__);
       msg->desc.free_cbk(msg);
       return;
    }
}
#endif

void
cntlr_configure_switch(uint64_t dp_handle)
{
    struct of_msg  *msg;
    int32_t         retval= CNTLR_SUCCESS;
    int32_t         status= OF_SUCCESS;

    CNTLR_RCU_READ_LOCK_TAKE();

    do
    {
        msg = ofu_alloc_message(OFPT_SET_CONFIG,
                                OFU_SWITCH_CONFIG_SET_REQ_MESSAGE_LEN);
        if(msg == NULL)
        {
           OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
           status = OF_FAILURE;
           break;
        }

        /*Configure switch to handle frags normally, no-reassembly*/
        /*TBD configuration support is required to take input*/
        ofu_set_switch_to_frag_normal(msg);

        /*Configure switch to miss send length as default, i.e., 126 bytes*/
        /*TBD configuration support is required to take input*/
        ofu_set_miss_send_len_in_switch(msg, OF_DEFAULT_SW_MISS_SEND_LEN);

        printf("%s:%d dp_handle in channel %llx\r\n",__FUNCTION__,__LINE__,dp_handle);
        retval = of_set_switch_config_msg(msg,
                                          dp_handle,
                                          NULL,
                                          NULL);
        if(retval != CNTLR_SUCCESS)
        {
           OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Set Switch config fail, error=%d \r\n",
                                                   __FUNCTION__,__LINE__,retval);
           status = OF_FAILURE;
           break;
        }
    }
    while(0);

    CNTLR_RCU_READ_LOCK_RELEASE();

    if(status != OF_SUCCESS)
    {
       if(msg != NULL)
         msg->desc.free_cbk(msg);
    }     
}

int32_t config_resources()
{
   struct dprm_switch_info                         switch_1;
   struct dprm_distributed_forwarding_domain_info* domain_info_1;
   struct dprm_datapath_general_info               datapath_info_1;
   uint64_t                                        switch_handle_1 = 0;
   uint64_t                                        domain_handle_1     = 0;
   uint64_t                                        dp_handle_1         = 0;
   uint32_t                                        retval = OF_SUCCESS;
   uint32_t                                        status = OF_SUCCESS;

   do
   {
      /*Add switch to the system*/
      strcpy(switch_1.name, "OF-Switch-1");
      switch_1.switch_type        = PHYSICAL_SWITCH;
      switch_1.ipv4addr.baddr_set = FALSE;
      strcpy(switch_1.switch_fqdn,"http:/xyz.com.");
      retval = dprm_register_switch(&switch_1,&switch_handle_1);
      if(retval != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Add Switcht to the system fail,retval %d\r\n",__FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }

      /*Add domain to the system*/
      domain_info_1 =  calloc(1,
                     (sizeof (struct dprm_distributed_forwarding_domain_info) + 2*sizeof(struct dprm_table_info)) );
      if(domain_info_1 == NULL)
      { 
         OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Mallocm of domain info failed retval=%d\r\n",__FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      } 
      strcpy(domain_info_1->name,"DOMAIN-1");
      strcpy(domain_info_1->expected_subject_name_for_authentication,"domain_1@xyz.com");
      domain_info_1->number_of_tables = FLOW_APP_NO_OF_TABLES;
      /*TBD, Table details*/
      retval = dprm_create_distributed_forwarding_domain(&domain_info_1,&domain_handle_1);
      if(retval != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Add Domain to the system fail retval=%d\r\n",__FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }

      /* Add Data path to the system*/
      datapath_info_1.dpid = 0x0000000a12345678;
      strcpy(datapath_info_1.expected_subject_name_for_authentication,"dp_1@xyz.com");
      retval = dprm_register_datapath(&datapath_info_1,switch_handle_1,domain_handle_1,&dp_handle_1);
      if(retval != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Add Data path to the system fail retval=%d\r\n",__FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }

      retval = of_register_symmetric_message_hooks(domain_handle_1,
                                                   OF_SYMMETRIC_MSG_ECHO_REQ,
                                                   (void*)of_handle_echo_request_msg,
                                                   NULL,
                                                   NULL);
      if(retval != OF_SUCCESS)
      {
          OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Register to receive echo request message failed\r\n",__FUNCTION__,__LINE__);
          status = OF_FAILURE;
          break;
      }
   }
   while(0);
   /*TBD cleanup*/

  return status;
}
#endif
