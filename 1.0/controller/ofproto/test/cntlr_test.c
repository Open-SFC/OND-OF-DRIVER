#include "controller.h"
#include "of_utilities.h"
#include "of_multipart.h"
#include "of_msgapi.h"
#include "cntrl_queue.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_event.h"
#include "dprmldef.h"
#include "cntlr_sample_app.h"


extern int32_t
cntlr_app_init(uint64_t  domain_handle);

extern uint32_t       no_of_data_paths_attached_g;

extern void
of_handle_echo_request_msg(uint64_t controller_handle,
                           uint64_t domain_handle,
                           uint64_t datapath_handle,
                           struct of_msg  *msg,
                           struct ofi_echo_req_recv_msg *echo_req,
                           void *cbk_arg1,
                           void *cbk_arg2);

extern void sm_app_init(uint64_t  domain_handle);

#if 0
void
add_flow_entry_fwd_table(uint64_t datapath_handle,
                         uint32_t source_ip,
                         uint32_t dest_ip)
{
   struct of_msg *msg;
   uint16_t       msg_len;
   uint16_t      instruction_len;
   uint8_t       *inst_start_loc;
   uint16_t      action_len;
   uint8_t       *action_start_loc;
   uint16_t      match_fd_len;
   uint8_t       *match_start_loc;
   uint16_t      flags;
   uint32_t      dst_msk_val;
   uint32_t      out_port;
   uint16_t      dst_port;
   int32_t       retval = OF_SUCCESS;
   int32_t       status = OF_SUCCESS;
   void *conn_info_p;
  
   msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+8+ 8+
              OFU_ETH_TYPE_FIELD_LEN+
              OFU_IPV4_DST_FIELD_LEN+OFU_IPV4_SRC_FIELD_LEN+OFU_META_DATA_FIELD_LEN+
              OFU_META_DATA_MASK_LEN+
              OFU_APPLY_ACTION_INSTRUCTION_LEN+OFU_SET_FIELD_WITH_ETH_DST_MAC_ACTION_LEN+OFU_OUTPUT_ACTION_LEN);

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
     
      ofu_start_setting_match_field_values(msg);

      uint16_t eth_type = 0x800; /*Eth type is IP*/
      retval = of_set_ether_type(msg,&eth_type);
      if(retval != OFU_SET_FIELD_SUCCESS)
      {
          OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d OF Set IPv4 src field failed,err = %d\r\n",__FUNCTION__,__LINE__,retval);
          status = OF_FAILURE;
          break;
      } 
      retval = of_set_ipv4_source(msg,&source_ip,FALSE,NULL);
      if(retval != OFU_SET_FIELD_SUCCESS)
      {
          OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d OF Set IPv4 src field failed,err = %d\r\n",__FUNCTION__,__LINE__,retval);
          status = OF_FAILURE;
          break;
      } 

      dst_msk_val = 0xffffff00;
      retval = of_set_ipv4_destination(msg,&dest_ip,TRUE,&dst_msk_val);
      if(retval != OFU_SET_FIELD_SUCCESS)
      {
          OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d OF Set IPv4 dest field failed,err = %d\r\n",__FUNCTION__,__LINE__,retval);
          status = OF_FAILURE;
          break;
      }

      dst_port = 1213;
      retval = of_set_tcp_source_port(msg,&dst_port);
      if(retval != OFU_SET_FIELD_SUCCESS)
      {
          OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d OF Set IPv4 dest field failed,err = %d\r\n",__FUNCTION__,__LINE__,retval);
          status = OF_FAILURE;
          break;
      }
      ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);

      ofu_start_pushing_instructions(msg);

      uint64_t meta_data = (uint64_t)dest_ip;
      uint64_t meta_data_mask = (uint64_t)0xffffffff;
      retval = ofu_push_write_meta_data_instruction(msg,meta_data,meta_data_mask);
      if(retval != OFU_INSTRUCTION_PUSH_SUCCESS)
      {
          OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d  Error in adding apply action instruction err = %d\r\n",
                                                      __FUNCTION__,__LINE__,retval);
          status = OF_FAILURE;
          break;
      }

      retval = ofu_start_pushing_apply_action_instruction(msg);
      if(retval != OFU_INSTRUCTION_PUSH_SUCCESS)
      {
          OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d  Error in adding apply action instruction err = %d\r\n",
                                                      __FUNCTION__,__LINE__,retval);
          status = OF_FAILURE;
          break;
      }

      uint8_t dest_mac[6] = {0x00,0x07,0xe9,0x09,0xfa,0xd7};
      retval = ofu_push_set_destination_mac_in_set_field_action(msg, dest_mac);
      if(retval != OFU_ACTION_PUSH_SUCCESS)
      {
          OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d  Error in adding set field action  err = %d\r\n",
                                                      __FUNCTION__,__LINE__,retval);
          status = OF_FAILURE;
          break;
      }
 
      out_port = 2;
      retval = ofu_push_output_action(msg,out_port,0);
      if(retval != OFU_ACTION_PUSH_SUCCESS)
      {
          OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d  Error in adding output action  err = %d\r\n",
                                                      __FUNCTION__,__LINE__,retval);
          status = OF_FAILURE;
          break;
      }
      ofu_end_pushing_actions(msg,action_start_loc,&action_len);


      ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);

      flags = 0;
      flags |= OFPFF_RESET_COUNTS;

      retval = of_create_add_flow_entry_msg(msg,
                                          datapath_handle,
                                          0, /*Table Id 0*/
                                          100, /*priority*/
                                          OFPFC_ADD,
                                          0, /*coockie*/
                                          0,/*Coockie amsk*/
                                          OFP_NO_BUFFER,
                                          flags,
                                          0, /*No Hard timeout*/
                                          0, /* No Idle timeout*/
                                           &conn_info_p);
     if(retval != OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS)
     {
          OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d  Error in adding miss entry  = %d\r\n",
                                                      __FUNCTION__,__LINE__,retval);
          status = OF_FAILURE;
     }
                                          
   }
   while(0);

   if(status == OF_FAILURE)
   {
      if(msg != NULL)
        msg->desc.free_cbk(msg);
   }
    
}

int32_t
fwd_app_pkt_rcvd(uint64_t              controller_handle,
                uint64_t               domain_handle,
                uint64_t               datapath_handle,
                struct of_msg         *msg,
                struct ofl_packet_in  *pkt_in,
                void                  *cbk_arg1,
                void                  *cbk_arg2)
{
    uint32_t source_ip;
    uint32_t dest_ip;
#if 0
    printf(" ########### %s #########\r\n",__FUNCTION__);
    printf("Received pkt of length %d\r\n",pkt_in->packet_length);
   {

     unsigned char *ptr = pkt_in->packet;
     uint32_t ii;
     for(ii = 0; ii < (pkt_in->packet_length) ;ii++,ptr++)
                 printf("%x ",*ptr);
      printf("\r\n");
   }
#endif

   source_ip = ntohl(*(uint32_t*)(pkt_in->packet + 14+12)); 
   dest_ip = ntohl(*(uint32_t*)(pkt_in->packet + 14+16)); 


#if 0
   if((source_ip == 0xc0a80102) && (dest_ip == 0xc0a80202))
   {
#endif
       add_flow_entry_fwd_table(datapath_handle,source_ip,dest_ip);
  // }

  msg->desc.free_cbk(msg);

 return OF_ASYNC_MSG_CONSUMED;
}

int32_t
fwd_app_dp_ready_event(uint64_t  controller_handle,
                       uint64_t   domain_handle,
                       uint64_t   datapath_handle,
                       void       *cbk_arg1,
                       void       *cbk_arg2)
{
   struct of_msg *msg;
   uint16_t       msg_len;
   uint16_t      instruction_len;
   uint8_t       *inst_start_loc;
   uint16_t      action_len;
   uint8_t       *action_start_loc;
   uint16_t      match_fd_len;
   uint8_t       *match_start_loc;
   uint16_t      flags;
   int32_t       retval = OF_SUCCESS;
   int32_t       status = OF_SUCCESS;
   void *conn_info_p;
  
   msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+ 8+ 8 +
              OFU_APPLY_ACTION_INSTRUCTION_LEN+OFU_OUTPUT_ACTION_LEN);

   OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_DEBUG, "Msg Len %d\r\n",msg_len);
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
     
       /*TBD, if we are not pusshing any match fields,this might be not rquired*/ 
       ofu_start_setting_match_field_values(msg);
       ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);

       OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_DEBUG, "Match field len %d\r\n",match_fd_len);

       ofu_start_pushing_instructions(msg, match_fd_len);
     
      retval = ofu_start_pushing_apply_action_instruction(msg);
      if(retval != OFU_INSTRUCTION_PUSH_SUCCESS)
      {
          OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d  Error in adding apply action instruction err = %d\r\n",
                                                      __FUNCTION__,__LINE__,retval);
          status = OF_FAILURE;
          break;
      }

      retval = ofu_push_output_action(msg,OFPP_CONTROLLER,OFPCML_NO_BUFFER);
      if(retval != OFU_ACTION_PUSH_SUCCESS)
      {
          OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d  Error in adding output action  err = %d\r\n",
                                                      __FUNCTION__,__LINE__,retval);
          status = OF_FAILURE;
          break;
      }
      ofu_end_pushing_actions(msg,action_start_loc,&action_len);

      OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_DEBUG,"action len %d\r\n",action_len);

      ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);
      OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_DEBUG,"Instruction len %d\r\n",instruction_len);

      flags = 0;
      flags |= OFPFF_RESET_COUNTS;

      OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_DEBUG,"Total msg len %d",msg->desc.data_len);
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
                                          &conn_info_p);
     if(retval != OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS)
     {
          OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d  Error in adding miss entry  = %d\r\n",
                                                      __FUNCTION__,__LINE__,retval);
          status = OF_FAILURE;
     }
                                          
   }
   while(0);

   if(status == OF_FAILURE)
   {
      if(msg != NULL)
        msg->desc.free_cbk(msg);
   }
    
   return EVENT_CONSUMED_BY_APP;
}

int32_t
app_init(uint64_t  domain_handle)
{
      int32_t retval = OF_SUCCESS;

      if(cntlr_register_asynchronous_event_hook(domain_handle,
                                                DP_READY_EVENT,
                                                1, 
                                                fwd_app_dp_ready_event,
                                                NULL,
                                                NULL) == OF_FAILURE)
      {
          OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Register DP Ready event cbk failed\r\n",__FUNCTION__,__LINE__);
          return OF_FAILURE;
      }


      if(of_register_asynchronous_message_hook(domain_handle,
                                               FWD_APP_ROUTE_TABLE_ID,
                                               OF_ASYNC_MSG_PACKET_IN_EVENT,
                                               FWD_APP_PKT_IN_PRIORITY,
                                               (void*)fwd_app_pkt_rcvd,NULL,NULL) == OF_FAILURE)
      {
          OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Register Forwarding app to receive pkt in failed\r\n",__FUNCTION__,__LINE__);
          return OF_FAILURE;
      }

      retval = of_register_symmetric_message_hooks(domain_handle,
                                                   OF_SYMMETRIC_MSG_ECHO_REQ,
                                                   (void*)of_handle_echo_request_msg,
                                                   NULL,
                                                   NULL);
      if(retval != OF_SUCCESS)
      {
          OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Register to receive echo request message failed\r\n",__FUNCTION__,__LINE__);
          return OF_FAILURE;
      }

    return OF_SUCCESS;
}
#endif

#if 0
int32_t config_resources()
{
   struct dprm_switch_info                         switch_1;
   struct dprm_distributed_forwarding_domain_info* domain_info_1;
   struct dprm_datapath_general_info               datapath_info_1;
   uint32_t                                        domain_info_size;
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
     
      printf("Switch Handle %llx\r\n",switch_handle_1);

      domain_info_size = sizeof(struct dprm_distributed_forwarding_domain_info) + FWD_APP_NO_OF_TABLES*sizeof(struct dprm_table_info);

      /*Add domain to the system*/
      domain_info_1 = (struct dprm_distributed_forwarding_domain_info*) calloc(1,domain_info_size);
      if(domain_info_1 == NULL)
      { 
         OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Mallocm of domain info failed retval=%d\r\n",__FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      } 
      strcpy(domain_info_1->name,"DOMAIN-1");
      strcpy(domain_info_1->expected_subject_name_for_authentication,"domain_1@xyz.com");
      domain_info_1->number_of_tables = FWD_APP_NO_OF_TABLES;
      strncpy(domain_info_1->tables[0].name,"ROUTE_TABLE",DPRM_MAX_TABLE_LEN);
      domain_info_1->tables[0].table_id = FWD_APP_ROUTE_TABLE_ID;
      strncpy(domain_info_1->tables[1].name,"ARP_TABLE",DPRM_MAX_TABLE_LEN);
      domain_info_1->tables[1].table_id = FWD_APP_ARP_TABLE_ID;
      retval = dprm_create_distributed_forwarding_domain(domain_info_1,&domain_handle_1);
      if(retval != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Add Domain to the system fail retval=%d\r\n",__FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }

       printf("Domain Handle %llx\r\n",domain_handle_1);

      /* Add Data path to the system*/
      datapath_info_1.dpid = 0xa12345678;
      strcpy(datapath_info_1.expected_subject_name_for_authentication,"dp_1@xyz.com");
      retval = dprm_register_datapath(&datapath_info_1,switch_handle_1,domain_handle_1,&dp_handle_1);
      if(retval != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Add Data path to the system fail retval=%d\r\n",__FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }
       printf("Datapath Handle %llx\r\n",dp_handle_1);
#if 0

      if(of_register_asynchronous_message_hook(domain_handle_1,
                                               0,
                                               OF_ASYNC_MSG_PACKET_IN_EVENT,
                                               1,
                                               (void*)pkt_out_test_func,NULL,NULL) == OF_FAILURE)
      {
          OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Register pkt out test routine failed\r\n",__FUNCTION__,__LINE__);
          status = OF_FAILURE;
          break;
      }
#endif

      cntlr_app_init(domain_handle_1);

      //app_init(domain_handle_1);
  }
  while(0);

  if(status == OF_FAILURE)
  {
      /*TBD release of all resources*/

    return OF_FAILURE;
  }

   return OF_SUCCESS;
}
#endif


int32_t config_resources()
{
   struct dprm_switch_info                         switch_1;
   struct dprm_distributed_forwarding_domain_info  domain_info_1;
   struct dprm_datapath_general_info               datapath_info_1 = {};
   struct dprm_datapath_general_info               datapath_info_2 = {};
   struct dprm_datapath_general_info               datapath_info_3 = {};
   uint64_t                                        switch_handle_1 = 0;
   uint64_t                                        domain_handle_1     = 0;
   uint64_t                                        dp_handle_1         = 0;
   uint64_t                                        dp_handle_2         = 0;
   uint64_t                                        dp_handle_3         = 0;
   uint32_t                                        dp_index = 0;
   uint32_t                                        retval = OF_SUCCESS;
   uint32_t                                        status = OF_SUCCESS;

   do
   {
    #if 1
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
     

      strcpy(domain_info_1.name,"DOMAIN-1");
      strcpy(domain_info_1.expected_subject_name_for_authentication,"domain_1@xyz.com");
      retval = dprm_create_distributed_forwarding_domain(&domain_info_1,&domain_handle_1);
      if(retval != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Add Domain to the system fail retval=%d\r\n",__FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }


      //struct dprm_table_info table_info;
      struct dprm_table_info oftable_info = {};
      struct dprm_oftable_entry *oftable_entry = NULL;
      uint64_t  table_handle;

      /* This table is to forward ARP request packets to ARP to application with PKT-IN msg.*/
      memset(&oftable_info, 0, sizeof(struct dprm_table_info));
      strcpy(oftable_info.table_name,"ARP-APP-TABLE");
      oftable_info.table_id           = SM_ARP_APP_TABLE_ID;/* Table 0*/
      oftable_info.no_of_match_fields = 1;
      retval = dprm_add_oftable_to_domain(domain_handle_1,&oftable_info,&table_handle);
      if(retval != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Add table to domain fail, retval=%d\r\n",__FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }

      retval = get_domain_table_byname(domain_handle_1,"ARP-APP-TABLE",&oftable_entry);
      if(retval != DPRM_SUCCESS)
      {
         printf("\r\n%s: Failed to get the table structure",__FUNCTION__);
         status = OF_FAILURE;
         break;
      }

      retval = add_domain_oftable_to_idbased_list(domain_handle_1,
                               SM_ARP_APP_TABLE_ID,oftable_entry);
      if(retval != DPRM_SUCCESS)
      {
         printf("\r\n%s: Failed to add ARP table to table list",__FUNCTION__);
         status = OF_FAILURE;
         break;
      }

      /* Add Session Table to the domain */ 
      oftable_entry = NULL;
      strcpy(oftable_info.table_name,"SESSION-TABLE");
      oftable_info.table_id           = SM_APP_SESSION_TABLE_ID;
      oftable_info.no_of_match_fields = 6;
      retval = dprm_add_oftable_to_domain(domain_handle_1,&oftable_info,&table_handle);
      if(retval != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Add table to domain fail, retval=%d\r\n",__FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }

      retval = get_domain_table_byname(domain_handle_1,"SESSION-TABLE",&oftable_entry);
      if(retval != DPRM_SUCCESS)
      {
         printf("\r\n%s: Failed to get the table structure",__FUNCTION__);
         status = OF_FAILURE;
         break;
      }

      retval = add_domain_oftable_to_idbased_list(domain_handle_1,
                          SM_APP_SESSION_TABLE_ID,oftable_entry);
      if(retval != DPRM_SUCCESS)
      {
         printf("\r\n%s: Failed to add SESSION table to table list",__FUNCTION__);
         status = OF_FAILURE;
         break;
      }

      /* Add Routing table to the domain */
      oftable_entry = NULL;
      memset(&oftable_info, 0, sizeof(struct dprm_table_info));
      strcpy(oftable_info.table_name,"ROUTE-TABLE");
      oftable_info.table_id           = SM_APP_L3ROUTING_TABLE_ID;
      oftable_info.no_of_match_fields = 2;
      retval = dprm_add_oftable_to_domain(domain_handle_1,&oftable_info,&table_handle);
      if(retval != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Add table to domain fail, retval=%d\r\n",__FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }

      retval = get_domain_table_byname(domain_handle_1,"ROUTE-TABLE",&oftable_entry);
      if(retval != DPRM_SUCCESS)
      {
         printf("\r\n%s: Failed to get the table structure",__FUNCTION__);
         status = OF_FAILURE;
         break;
      }

      retval = add_domain_oftable_to_idbased_list(domain_handle_1,
                         SM_APP_L3ROUTING_TABLE_ID,oftable_entry);
      if(retval != DPRM_SUCCESS)
      {
         printf("\r\n%s: Failed to add ROUTING table to table list",__FUNCTION__);
         status = OF_FAILURE;
         break;
      }

      oftable_entry = NULL;
      memset(&oftable_info, 0, sizeof(struct dprm_table_info));
      strcpy(oftable_info.table_name,"ARP-TABLE");
      oftable_info.table_id           = SM_APP_ARP_TABLE_ID;/* Table 3*/
      oftable_info.no_of_match_fields = 1;
      retval = dprm_add_oftable_to_domain(domain_handle_1,&oftable_info,&table_handle);
      if(retval != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Add table to domain fail, retval=%d\r\n",__FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }

      retval = get_domain_table_byname(domain_handle_1,"ARP-TABLE",&oftable_entry);
      if(retval != DPRM_SUCCESS)
      {
         printf("\r\n%s: Failed to get the table structure",__FUNCTION__);
         status = OF_FAILURE;
         break;
      }

      retval = add_domain_oftable_to_idbased_list(domain_handle_1,
                           SM_APP_ARP_TABLE_ID,oftable_entry);
      if(retval != DPRM_SUCCESS)
      {
         printf("\r\n%s: Failed to add ARP table to table list",__FUNCTION__);
         status = OF_FAILURE;
         break;
      }

      /* Add Data path to the system*/
      datapath_info_1.dpid = 0x020001020304;
      strcpy(datapath_info_1.expected_subject_name_for_authentication,"dp_1@xyz.com");
      retval = dprm_register_datapath(&datapath_info_1,switch_handle_1,domain_handle_1,&dp_handle_1);
      if(retval != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Add Data path to the system fail retval=%d\r\n",__FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }
      printf("Datapath Handle-1 %llx for switch \r\n",dp_handle_1);
      sm_app_attach_dpid_to_conf_files(dp_index, datapath_info_1.dpid, dp_handle_1);
   
      dp_index++;
      datapath_info_2.dpid = 0x030001020304;
      strcpy(datapath_info_2.expected_subject_name_for_authentication,"dp_1@xyz.com");
      retval = dprm_register_datapath(&datapath_info_2,switch_handle_1,domain_handle_1,&dp_handle_2);
      if(retval != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Add Data path to the system fail retval=%d\r\n",__FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }
      printf("Datapath Handle-2 %llx for switch \r\n",dp_handle_2);
      sm_app_attach_dpid_to_conf_files(dp_index, datapath_info_2.dpid, dp_handle_2);

      dp_index++;
      datapath_info_3.dpid = 0x040001020304;
      strcpy(datapath_info_3.expected_subject_name_for_authentication,"dp_1@xyz.com");
      retval = dprm_register_datapath(&datapath_info_3,switch_handle_1,domain_handle_1,&dp_handle_3);
      if(retval != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_CNTLR_TEST, OF_LOG_ERROR,"%s:%d Add Data path to the system fail retval=%d\r\n",__FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }
      printf("Datapath Handle-3 %llx for switch \r\n",dp_handle_3);
      sm_app_attach_dpid_to_conf_files(dp_index, datapath_info_3.dpid, dp_handle_3);
      no_of_data_paths_attached_g = dp_index + 1;
    #endif
      cntlr_app_init(domain_handle_1);
      sm_app_init(domain_handle_1);
  }
  while(0);

  if(status == OF_FAILURE)
  {
      /*TBD release of all resources*/
    return OF_FAILURE;
  }

  return OF_SUCCESS;
}

