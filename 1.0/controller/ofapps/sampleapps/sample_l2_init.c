#include "controller.h"
#include "cntlr_epoll.h"
#include "cntrl_queue.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "of_utilities.h"
#include "of_msgapi.h"
#include "cntlr_event.h"
#include <execinfo.h>
#include <sched.h>
#include "tmrldef.h"
#include "time.h"
#include "sll.h"
#include "memapi.h"
#include "memldef.h"
#include "dprmldef.h"
#include <sys/time.h>
#include "cntlr_sample_app.h"
#include "cntlr_l2_app.h"

uint32_t          		        	no_of_l2data_paths_attached_g ;
cntlr_sml2_sw_conf_t     			l2_dp_conf[OF_CNTLR_MAX_DATA_PATHS];
struct dprm_distributed_forwarding_domain_info  l2_fwd_domain_info;
uint32_t                                        ethaddr_entry_priority = 1 ;
uint32_t                                        mac_learn_entry_priority = 10 ;
uint32_t                                        fib_entry_priority = 10 ;

extern  int32_t sm_app_frame_and_send_pktout_to_dp(uint64_t datapath_handle, 
                                               struct ofl_packet_in *pkt_in);

/* 
 * Function prototypes 
 */
static void l2_app_domain_event_cbk(uint32_t notification_type,
    uint64_t domain_handle,
    struct   dprm_domain_notification_data notification_data,
    void     *callback_arg1, void     *callback_arg2) ;

int32_t l2_app_dp_ready_event(uint64_t  controller_handle,
    uint64_t   domain_handle, uint64_t   datapath_handle,
    void       *cbk_arg1, void       *cbk_arg2) ;

int32_t l2_app_mac_learn_handler(int64_t controller_handle,
    uint64_t domain_handle, uint64_t datapath_handle,
    struct of_msg *msg, struct ofl_packet_in  *pkt_in,
    void *cbk_arg1, void *cbk_arg2) ;

int32_t l2_app_fib_handler(int64_t controller_handle,
    uint64_t domain_handle, uint64_t datapath_handle,
    struct of_msg *msg, struct ofl_packet_in  *pkt_in,
    void *cbk_arg1, void *cbk_arg2) ;

int32_t l2_app_mac_learn_pkt_process(struct of_app_msg *app_msg) ;

static int32_t l2_app_add_proactive_flow_to_ethaddr_type(uint64_t datapath_handle) ;
static int32_t l2_app_add_miss_flow_to_ethaddr_type(uint64_t datapath_handle) ;

static int32_t l2_app_add_miss_flow_to_mac_learn(uint64_t datapath_handle, uint64_t );

static int32_t l2_app_add_miss_flow_to_fib(uint64_t datapath_handle) ;

static int32_t l2_app_add_miss_flow_to_bcast_table(uint64_t datapath_handle, uint64_t ) ;

int32_t l2_app_add_mac_learn_entry(uint64_t , uint8_t  *, uint32_t ) ;

int32_t l2_app_add_fib_entry(uint64_t , uint8_t  *, uint32_t ) ;
int32_t l2_app_fib_pkt_process(struct of_app_msg *app_msg) ;

/*
 * Function definitions
 */
void sample_l2_init(void)
{
  uint32_t dp_index;

  strcpy(l2_fwd_domain_info.name,"L2_FWD_DOMAIN");
  strcpy(l2_fwd_domain_info.expected_subject_name_for_authentication,"l2fwd_domain@abc.com");

  if (dprm_register_forwarding_domain_notifications(DPRM_DOMAIN_ALL_NOTIFICATIONS,
        l2_app_domain_event_cbk, &l2_fwd_domain_info, NULL) != OF_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Registering callback to L2 forward domain failed....");
    return;
  }
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Registering callback to L2 forward domain Successful....");

  for (dp_index = 0;dp_index < OF_CNTLR_MAX_DATA_PATHS; dp_index++)
  {
    l2_dp_conf[dp_index].no_of_ethaddr_type_entries = 0;
    l2_dp_conf[dp_index].no_of_mac_learn_entries = 0;
    l2_dp_conf[dp_index].no_of_fib_entries = 0;
    l2_dp_conf[dp_index].no_of_bcast_entries = 0;
  }
  return;
}

static void l2_app_domain_event_cbk(uint32_t notification_type,
            uint64_t domain_handle, struct   dprm_domain_notification_data notification_data,
            void     *callback_arg1, void     *callback_arg2)
{ 
  uint64_t dpid;
  struct dprm_datapath_entry *pdatapath_info;
  uint64_t datapath_handle = notification_data.datapath_handle;
  struct dprm_distributed_forwarding_domain_info  *domain_info=(struct dprm_distributed_forwarding_domain_info *)callback_arg1 ;
  uint32_t  dpinfo_index ;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"domain name = %s  domain name of our intrest %s",
                   notification_data.domain_name, domain_info->name);

  /* L2_FWD_DOMAIN belongs to "SAMPLE_L2_FWD_TTP"*/
  if(strcmp(notification_data.ttp_name, "SAMPLE_L2_FWD_TTP"))
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"notification is  not for l2_fwd_ttp.");
    return;
  }

  switch(notification_type)
  {
    case DPRM_DOMAIN_ADDED:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"DPRM_DOMAIN_ADDED");
        if (cntlr_register_asynchronous_event_hook(domain_handle,
              DP_READY_EVENT, 1, l2_app_dp_ready_event,
              NULL, NULL) == OF_FAILURE)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Register DP Ready event cbk failed");
        }
	if (of_register_asynchronous_message_hook(domain_handle,
				L2_APP_MAC_LEARN_TABLE_ID,/*Table 1*/
				OF_ASYNC_MSG_PACKET_IN_EVENT,
				SM_APP_PKT_IN_PRIORITY,
				l2_app_mac_learn_handler, NULL,NULL) != OF_SUCCESS)
	{
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"registering MAC LEARN  Reply cbk");
	}

	if (of_register_asynchronous_message_hook(domain_handle,
				L2_APP_FIB_TABLE_ID,/*Table 2*/
				OF_ASYNC_MSG_PACKET_IN_EVENT,
				SM_APP_PKT_IN_PRIORITY,
				l2_app_fib_handler, NULL,NULL) != OF_SUCCESS)
	{  
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"registering FIB  Reply cbk");
	}
	break;
      }
    case DPRM_DOMAIN_DELETE:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"DPRM_DOMAIN_DELETED");
        break;
      }

      case DPRM_DOMAIN_DATAPATH_ATTACHED:
      {
        if (get_datapath_byhandle(datapath_handle, &pdatapath_info) != DPRM_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "No DP Entry with given datapath_handle=%llu",datapath_handle);
	  break;
        }
        dpid = pdatapath_info->dpid;
        dpinfo_index = no_of_l2data_paths_attached_g;
        no_of_l2data_paths_attached_g++;
        l2_dp_conf[dpinfo_index].dp_id = dpid;
        l2_dp_conf[dpinfo_index].dp_handle = datapath_handle;
        l2_dp_conf[dpinfo_index].dp_index = dpinfo_index;
        break;
      }
    case DPRM_DOMAIN_DATAPATH_DETACH:
      {
        if (get_datapath_byhandle(datapath_handle, &pdatapath_info) != DPRM_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN, "No DP Entry with given datapath_handle=%llu",datapath_handle);
	  break;
        }
        dpid = pdatapath_info->dpid;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," DPRM_DOMAIN_DATAPATH_DETACHED datapath handle=%llu dpid=%llx noOfDps=%d",
            datapath_handle,dpid,no_of_l2data_paths_attached_g);
        no_of_l2data_paths_attached_g--;
        break;
      }
    default:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN, "Invalid domain event=%d",notification_type);
      }
  }
  return;
}

int32_t l2_app_dp_ready_event(uint64_t  controller_handle,
        uint64_t   domain_handle, uint64_t   datapath_handle,
        void       *cbk_arg1, void       *cbk_arg2)
{
  struct dprm_datapath_entry *pdatapath_info;
  int32_t status = OF_SUCCESS;
  uint32_t dp_index;
  uint64_t  dpid ;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"DATA PATH READY EVENT RECEIVED");
  if (get_datapath_byhandle(datapath_handle, &pdatapath_info) != DPRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN, "No DP Entry with given datapath_handle=%llu",datapath_handle);
    return OF_FAILURE;
  }

  dpid = pdatapath_info->dpid ;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," dpid is : %llx", dpid);
 
  for (dp_index = 0; dp_index < no_of_l2data_paths_attached_g; dp_index++)
  {
    if (l2_dp_conf[dp_index].dp_handle == datapath_handle)
    {
      break;
    }
  }

  if (dp_index == no_of_l2data_paths_attached_g)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN, "No Datapath found with handle = %llx", datapath_handle);
    return OF_FAILURE;
  }

  do
  {
    /* Add pro active and miss entries in all tables */

    /* Table 0 ------------------------------------------------------------------  */
    /* Add a broadcast/multicast ethaddr type flow entries in TABLE0 */
    if (l2_app_add_proactive_flow_to_ethaddr_type(datapath_handle) != OF_SUCCESS)
    {
      status = OF_FAILURE;
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," L2 App table 0 : failed adding proactive flow entry");
      break;
    }
    /* Add a table miss entry in TABLE0 */
    if (l2_app_add_miss_flow_to_ethaddr_type(datapath_handle) != OF_SUCCESS)
    {
      status = OF_FAILURE;
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," L2 App table 0 : failed adding miss flow entry");
      break;
    }

    /* Table 1 ------------------------------------------------------------------  */
    /* Add a table miss entry in mac learn TABLE1 */
    if (l2_app_add_miss_flow_to_mac_learn(datapath_handle, dpid) != OF_SUCCESS)
    {
      status = OF_FAILURE;
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," L2 App table 1 : failed adding miss flow entry");
      break;
    }

    /* Table 2 ------------------------------------------------------------------  */
    /* Add a table miss entry in fib TABLE2 */
    if (l2_app_add_miss_flow_to_fib(datapath_handle) != OF_SUCCESS)
    {
      status = OF_FAILURE;
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," L2 App table 2 : failed adding miss flow entry");
      break;
    }

    /* Table 3  ----------------------------------------------------------------   */
    /* Add proctive entries into the Bcast/Mcast table TABLE 3 */
    if (l2_app_add_miss_flow_to_bcast_table(datapath_handle, dpid) != OF_SUCCESS)
    {
      status = OF_FAILURE;
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," L2 App table 3: failed adding proactive flow entry");
      break;
    }

  }
  while(0);

  return status;
}

static int32_t l2_app_add_proactive_flow_to_ethaddr_type(uint64_t dp_handle)
{
  struct      of_msg *msg=NULL;
  uint16_t    msg_len;
  uint16_t    instruction_len;
  uint8_t     *inst_start_loc = NULL;
  //uint16_t    action_len;
  //uint8_t     *action_start_loc = NULL;
  uint8_t     *match_start_loc = NULL;
  uint16_t    match_fd_len=0;
  uint16_t    flags;
  int32_t     retval = OF_SUCCESS;
  int32_t     status = OF_SUCCESS;
  uint8_t     dest_addr[6] =  {0x01, 0x00, 0x00, 0x00, 0x00, 0x00} ;
  //uint8_t     dest_addr[6] =  {0x01, 0x00, 0x00, 0x00, 0x00, 0x00} ;
  //uint8_t     dest_mask[6] =  {0xff, 0xff, 0xff, 0xff, 0xff, 0xff} ;
  uint8_t     dest_mask[6] =  {0x01, 0x00, 0x00, 0x00, 0x00, 0x00} ;

  void *conn_info_p;

  msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
        OFU_ETH_DST_MAC_ADDR_FIELD_LEN+
        OFU_GOTO_TABLE_INSTRUCTION_LEN + 32 ); /* 32 bytes extra */
  do
  {
    msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
    if (msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      break;
    }

    flags = 0;
    flags |= OFPFF_RESET_COUNTS;

    /* zero idle and hard timeout makes entry permanet */
    retval = of_create_add_flow_entry_msg(msg,
        dp_handle,
        L2_APP_ETHADDR_TYPE_TABLE_ID, /*Table Id 1*/
        ethaddr_entry_priority, /*priority*/
        OFPFC_ADD,
        0, /*coockie*/
        0,/*Coockie amsk*/
        OFP_NO_BUFFER,
        flags,
        0, /*No Hard timeout*/
        0, /*No Idle timeout*/
        &conn_info_p
        );

    if (retval != OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error adding flow entry in route table = %d",retval);
      status = OF_FAILURE;
    }

    /* Pushing Destination Address match Field */
    ofu_start_setting_match_field_values(msg);
    retval = of_set_destination_mac(msg, dest_addr, 1, dest_mask);
    if(retval != OFU_SET_FIELD_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF Set IPv4 src field failed,err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_setting_match_field_values(msg, match_start_loc, &match_fd_len);

    /* Pushing Goto table Instruction, in this case to BCAST Table */
    ofu_start_pushing_instructions(msg, match_fd_len);
    retval = ofu_push_go_to_table_instruction(msg, L2_APP_BCAST_TABLE_ID);
    if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"go to table instruction failed err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"instruction len = %d", instruction_len);

    retval=of_cntrlr_send_request_message(
        msg,
        dp_handle,
        conn_info_p,
        NULL,
        NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error adding flow entry in route table = %d",retval);
      status = OF_FAILURE;
    }
  }
  while(0);
  return status;
}

static int32_t l2_app_add_miss_flow_to_ethaddr_type(uint64_t datapath_handle)
{
  struct      of_msg *msg;
  uint16_t    msg_len;
  uint16_t    instruction_len =0;
  uint8_t     *inst_start_loc = NULL;
  uint16_t    match_fd_len=0;
  uint8_t     *match_start_loc = NULL;
  uint16_t    flags;
  int32_t     retval = OF_SUCCESS;
  int32_t     status = OF_SUCCESS;
  void *conn_info_p;

  msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
      OFU_APPLY_ACTION_INSTRUCTION_LEN+OFU_OUTPUT_ACTION_LEN + 32 ); /* To avoid any over writes, for now */

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg len %d", msg_len);

  do
  {
    /* Add Miss Entry to the ETH ADDR TYPE Table...*/
    msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
    if (msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      break;
    }

    flags = 0;
    flags |= OFPFF_RESET_COUNTS;
    retval = of_create_add_flow_entry_msg(msg,
        datapath_handle,
        L2_APP_ETHADDR_TYPE_TABLE_ID, /*Table Id 2*/
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

    if (retval != OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding miss entry MAC LEARN table = %d",retval);
      status = OF_FAILURE;
    }

    /*TBD, if we are not pusshing any match fields,this might be not rquired*/ 
    ofu_start_setting_match_field_values(msg);
    ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Match field len %d",match_fd_len);

    /* Pushing Goto table Instruction, in this case to MAC LEARN Table */
    ofu_start_pushing_instructions(msg, match_fd_len);
    retval = ofu_push_go_to_table_instruction(msg, L2_APP_MAC_LEARN_TABLE_ID);
    if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"go to table instruction failed err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"instruction len = %d total msg len %d", instruction_len, msg->desc.data_len);

    retval=of_cntrlr_send_request_message(
        msg,
        datapath_handle,
        conn_info_p,
        NULL,
        NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding miss entry arp table = %d",retval);
      status = OF_FAILURE;
    }
  }
  while(0);
  return status;
}

static int32_t
l2_app_add_miss_flow_to_mac_learn(uint64_t datapath_handle, uint64_t  dpid)
{
  struct      of_msg *msg;
  uint16_t    msg_len;
  uint16_t    instruction_len;
  uint8_t     *inst_start_loc = NULL;
  uint16_t    action_len;
  uint8_t     *action_start_loc = NULL;
  uint16_t    match_fd_len=0;
  uint8_t     *match_start_loc = NULL;
  uint16_t    flags;
  int32_t     retval = OF_SUCCESS;
  int32_t     status = OF_SUCCESS;
  void *conn_info_p;

  msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
      OFU_APPLY_ACTION_INSTRUCTION_LEN+OFU_OUTPUT_ACTION_LEN);

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg len %d", msg_len);
  do
  {
    /* Add Miss Entry to the MAC LEARN Table...*/
    msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
    if (msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      break;
    }

    flags = 0;
    flags |= OFPFF_RESET_COUNTS;
    retval = of_create_add_flow_entry_msg(msg,
        datapath_handle,
        L2_APP_MAC_LEARN_TABLE_ID, /*Table Id 1*/
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

    if (retval != OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding miss entry MAC LEARN table = %d",retval);
      status = OF_FAILURE;
    }

    /*TBD, if we are not pusshing any match fields,this might be not rquired*/ 
    ofu_start_setting_match_field_values(msg);
    ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Match field len %d",match_fd_len);

    ofu_start_pushing_instructions(msg, match_fd_len);
    retval = ofu_start_pushing_apply_action_instruction(msg);
    if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding apply action instruction err = %d",retval);
      status = OF_FAILURE;
      break;
    }

    if(dpid == 0xe)
      retval = ofu_push_output_action(msg,3, OFPCML_NO_BUFFER);
    else
      retval = ofu_push_output_action(msg,OFPP_CONTROLLER, OFPCML_NO_BUFFER);
    if (retval != OFU_ACTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding output action  err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_pushing_actions(msg,action_start_loc,&action_len);
    ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(action_len);
    ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);


    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"instruction len = %d total msg len %d", instruction_len, msg->desc.data_len);

    retval=of_cntrlr_send_request_message(
        msg,
        datapath_handle,
        conn_info_p,
        NULL,
        NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding miss entry arp table = %d",retval);
      status = OF_FAILURE;
    }
  }
  while(0);

  return status;
}

static int32_t l2_app_add_miss_flow_to_fib(uint64_t datapath_handle)
{
  struct      of_msg *msg;
  uint16_t    msg_len;
  uint16_t    instruction_len;
  uint8_t     *inst_start_loc = NULL;
  //uint16_t    action_len;
  //uint8_t     *action_start_loc = NULL;
  uint16_t    match_fd_len=0;
  uint8_t     *match_start_loc = NULL;
  uint16_t    flags;
  int32_t     retval = OF_SUCCESS;
  int32_t     status = OF_SUCCESS;
  void *conn_info_p;

  msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
      OFU_APPLY_ACTION_INSTRUCTION_LEN+OFU_OUTPUT_ACTION_LEN);

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg len %d", msg_len);
  do
  {
    /* Add Miss Entry to the MAC LEARN Table...*/
    msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
    if (msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      break;
    }

    flags = 0;
    flags |= OFPFF_RESET_COUNTS;
    retval = of_create_add_flow_entry_msg(msg,
        datapath_handle,
        L2_APP_FIB_TABLE_ID, /*Table Id 2*/
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

    if (retval != OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding miss entry MAC LEARN table = %d",retval);
      status = OF_FAILURE;
    }

    /*TBD, if we are not pusshing any match fields,this might be not rquired*/ 
    ofu_start_setting_match_field_values(msg);
    ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Match field len %d",match_fd_len);

    /* Pushing Goto table Instruction, in this case to BCAST Table */
    ofu_start_pushing_instructions(msg, match_fd_len);
    retval = ofu_push_go_to_table_instruction(msg, L2_APP_BCAST_TABLE_ID);
    if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"go to table instruction failed err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"instruction len = %d total msg len %d", instruction_len, msg->desc.data_len);

    retval=of_cntrlr_send_request_message(
        msg,
        datapath_handle,
        conn_info_p,
        NULL,
        NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding miss entry arp table = %d",retval);
      status = OF_FAILURE;
    }
  }
  while(0);

  return status;
}

static int32_t 
l2_app_add_miss_flow_to_bcast_table(uint64_t datapath_handle, uint64_t  dpid)
{
  struct      of_msg *msg;
  uint16_t    msg_len;
  uint16_t    instruction_len;
  uint8_t     *inst_start_loc = NULL;
  uint16_t    action_len;
  uint8_t     *action_start_loc = NULL;
  uint16_t    match_fd_len=0;
  uint8_t     *match_start_loc = NULL;
  uint16_t    flags=0;
  int32_t     retval = OF_SUCCESS;
  int32_t     status = OF_SUCCESS;
  void *conn_info_p;

  msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
        OFU_APPLY_ACTION_INSTRUCTION_LEN+OFU_OUTPUT_ACTION_LEN+32); 
                 /* 32 extra bytes, need to calculate exactly */

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Msg Len %d\r\n",msg_len);
  do
  {
    msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
    if (msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
      status = OF_FAILURE;
      break;
    }

    flags |= OFPFF_RESET_COUNTS;
    retval = of_create_add_flow_entry_msg(msg,
        datapath_handle,
        L2_APP_BCAST_TABLE_ID, /*Table Id 0*/
        0, /*priority*/
        OFPFC_ADD,
        0, /*coockie*/
        0,/*Coockie amsk*/
        OFP_NO_BUFFER,
        flags,
        0, /*No Hard timeout*/
        0, /* No Idle timeout*/
        &conn_info_p
        );

    if (retval != OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in adding arp flow entry  = %d",
          retval);
      status = OF_FAILURE;
    }

    ofu_start_setting_match_field_values(msg);
    ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);
    ofu_start_pushing_instructions(msg, match_fd_len);
    retval = ofu_start_pushing_apply_action_instruction(msg);
    if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d  Error in adding apply action instruction err = %d\r\n",
          __FUNCTION__,__LINE__,retval);
      status = OF_FAILURE;
      break;
    }

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," dpid is : %llx", dpid);

    if(dpid == 0xe)
      retval = ofu_push_output_action(msg, 3, OFPCML_NO_BUFFER);
    else
      retval = ofu_push_output_action(msg, OFPP_ALL, OFPCML_NO_BUFFER);
    if (retval != OFU_ACTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d  Error in adding output action  err = %d\r\n",
          __FUNCTION__,__LINE__,retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_pushing_actions(msg,action_start_loc,&action_len);
    ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(action_len);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"total actions len = %d for this instruction", action_len);

    ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);

    retval=of_cntrlr_send_request_message(
        msg,
        datapath_handle,
        conn_info_p,
        NULL,
        NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d  Error in adding arp flow entry  = %d\r\n",
          __FUNCTION__,__LINE__,retval);
      status = OF_FAILURE;
    }
  }
  while(0);
  return status;
}

/* For run time flow entries */
int32_t l2_app_mac_learn_handler(int64_t controller_handle,
    uint64_t domain_handle, uint64_t datapath_handle,
    struct of_msg *msg, struct ofl_packet_in  *pkt_in,
    void *cbk_arg1, void *cbk_arg2)
{
  uint8_t  *pkt_data = pkt_in->packet;
  struct dprm_datapath_entry *pdatapath_info;
  uint64_t  dpid;
  struct of_app_msg *app_msg; 
  int32_t iretval = OF_SUCCESS;

  if (get_datapath_byhandle(datapath_handle, &pdatapath_info) != DPRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN, "No DP Entry with given datapath_handle=%llu",datapath_handle);
  }
  dpid = pdatapath_info->dpid;

  app_msg=(struct of_app_msg *)calloc(1,sizeof(struct of_app_msg));
  app_msg->table_id=L2_APP_MAC_LEARN_TABLE_ID;
  app_msg->datapath_handle=datapath_handle;
  app_msg->controller_handle=controller_handle;
  app_msg->domain_handle=domain_handle;
  app_msg->dpid=dpid;
  app_msg->pkt_in=pkt_in;
  app_msg->pkt_data=pkt_data;
  app_msg->cbk_arg1=cbk_arg1;
  app_msg->cbk_arg2=cbk_arg2;

  iretval = l2_app_mac_learn_pkt_process(app_msg);
  free(app_msg);
  if (iretval != OF_SUCCESS)
  {
    return OF_ASYNC_MSG_DROP;
  }
  else
  {
    if (msg != NULL)
    {
      msg->desc.free_cbk(msg);
    }
    return OF_ASYNC_MSG_CONSUMED;
  }
}

int32_t l2_app_mac_learn_pkt_process(struct of_app_msg *app_msg)
{
  uint32_t in_port;
  uint8_t  *pkt_data;
  uint8_t  sender_hw_addr[OFP_ETH_ALEN];
  uint8_t  target_hw_addr[OFP_ETH_ALEN];
  uint8_t  *ucPtr;
  int32_t  status = OF_SUCCESS;
  int32_t  retval = OF_SUCCESS;
  uint64_t  dpid;
  uint32_t  dp_index;

  uint64_t datapath_handle;
  int64_t controller_handle;
  struct  of_msg  *msg=NULL;
  struct  ofl_packet_in  *pkt_in;
  uint64_t domain_handle;
  void *cbk_arg1;
  void *cbk_arg2;
  uint32_t ii;
  
  datapath_handle=  app_msg->datapath_handle;
  controller_handle=app_msg->controller_handle;
  domain_handle=app_msg->domain_handle;
  
  dpid=app_msg->dpid;
  pkt_in=app_msg->pkt_in;
  pkt_data = app_msg->pkt_data;

  cbk_arg1=app_msg->cbk_arg1;
  cbk_arg2=app_msg->cbk_arg2;

  for (dp_index = 0; dp_index < no_of_l2data_paths_attached_g; dp_index++)
  {
    if (l2_dp_conf[dp_index].dp_handle == datapath_handle)
      break;
  }

  if (dp_index == no_of_l2data_paths_attached_g)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"\n%s: No datapath found with given dp handle=%llx",__FUNCTION__,datapath_handle);
    return OF_FAILURE;
  }

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"PKT-IN RECEIVED FOR L2 MAC LEARN MISS ENTRY DPID=%llx dpindex=%d",dpid,dp_index);
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Received pkt of length %d",pkt_in->packet_length);

  /* Parse received packet */
  ucPtr = pkt_data;

  /* Skip Destination Mac Address */
  memcpy(target_hw_addr, ucPtr, OF_CNTLR_HW_ADDR_LEN);
  ucPtr = ucPtr + OFP_ETH_ALEN;

  /* parse sender mac address */
  memcpy(sender_hw_addr, ucPtr, OF_CNTLR_HW_ADDR_LEN);
  ucPtr += OFP_ETH_ALEN;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"MAC LEARN REQUEST");
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"PARSED PKT DETAILS");

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:target_HW: %x:%x:%x:%x:%x:%x",__FUNCTION__,target_hw_addr[0],target_hw_addr[1],target_hw_addr[2],target_hw_addr[3], target_hw_addr[4], target_hw_addr[5]);
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:sender_HW %x:%x:%x:%x:%x:%x",__FUNCTION__,sender_hw_addr[0],sender_hw_addr[1],sender_hw_addr[2],sender_hw_addr[3], sender_hw_addr[4], sender_hw_addr[5]);

  /* Get In Port */ 
  retval = ofu_get_in_port_field(msg, pkt_in->match_fields,
      pkt_in->match_fields_length, &in_port);
  if (retval != OFU_IN_PORT_FIELD_PRESENT)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:failed get inport ....inport=%d",__FUNCTION__,in_port);
    status = OF_FAILURE;
  }
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, " PKT-IN IN PORT =%d",in_port);

  do
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"PKT-IN RECEIVED FOR SESSION ENTRY MISS DPID=%llx",dpid);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Received pkt of length %d",pkt_in->packet_length);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg %p pkt_in %p pkt_data %p",app_msg,pkt_in,pkt_data);
	
#if 0	
    for (ii=0; ii< l2_dp_conf[dp_index].no_of_mac_learn_entries; ii++)
    {
      jj = 0 ;
	  	
      if ((l2_dp_conf[dp_index].mac_learn_table[ii].src_mac_addr[jj] == sender_hw_addr[jj++]) && 
	  (l2_dp_conf[dp_index].mac_learn_table[ii].src_mac_addr[jj] == sender_hw_addr[jj++]) &&
          (l2_dp_conf[dp_index].mac_learn_table[ii].src_mac_addr[jj] == sender_hw_addr[jj++]) && 
	  (l2_dp_conf[dp_index].mac_learn_table[ii].src_mac_addr[jj] == sender_hw_addr[jj++]) &&
	  (l2_dp_conf[dp_index].mac_learn_table[ii].src_mac_addr[jj] == sender_hw_addr[jj++]) && 
	  (l2_dp_conf[dp_index].mac_learn_table[ii].src_mac_addr[jj] == sender_hw_addr[jj++]))
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"packet matched Mac Learn rule... ****");
        break;
      }
    }
  
    if (ii == l2_dp_conf[dp_index].no_of_mac_learn_entries)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"packet did not match wall rule...");
      status = OF_FAILURE;
      break;
    }
#endif  
    if(l2_app_add_mac_learn_entry(datapath_handle, 
                   sender_hw_addr, in_port) != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"adding mac learn flow entry failed");
      status = OF_FAILURE;
      break;
    }

    if(l2_app_add_fib_entry(datapath_handle, 
                   sender_hw_addr, in_port) != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"adding mac learn flow entry failed");
      status = OF_FAILURE;
      break;
    }
	  					  
    if (sm_app_frame_and_send_pktout_to_dp(datapath_handle,pkt_in) != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"sending packet out failed");
      status = OF_FAILURE;
      break;
    }
  }
  while(0);
  
  return status;
}

int32_t l2_app_fib_handler(int64_t controller_handle,
    uint64_t domain_handle, uint64_t datapath_handle,
    struct of_msg *msg, struct ofl_packet_in  *pkt_in,
    void *cbk_arg1, void *cbk_arg2)
{
  uint8_t  *pkt_data = pkt_in->packet;
  struct dprm_datapath_entry *pdatapath_info;
  uint64_t  dpid;
  struct of_app_msg *app_msg; 
  int32_t iretval = OF_SUCCESS;

  if (get_datapath_byhandle(datapath_handle, &pdatapath_info) != DPRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN, "No DP Entry with given datapath_handle=%llu",datapath_handle);
  }
  dpid = pdatapath_info->dpid;

  app_msg=(struct of_app_msg *)calloc(1,sizeof(struct of_app_msg));
  app_msg->table_id=L2_APP_FIB_TABLE_ID;
  app_msg->datapath_handle=datapath_handle;
  app_msg->controller_handle=controller_handle;
  app_msg->domain_handle=domain_handle;
  app_msg->dpid=dpid;
  app_msg->pkt_in=pkt_in;
  app_msg->pkt_data=pkt_data;
  app_msg->cbk_arg1=cbk_arg1;
  app_msg->cbk_arg2=cbk_arg2;

  iretval = l2_app_fib_pkt_process(app_msg);
  free(app_msg);
  if (iretval != OF_SUCCESS)
  {
    return OF_ASYNC_MSG_DROP;
  }
  else
  {
    if (msg != NULL)
    {
      msg->desc.free_cbk(msg);
    }
    return OF_ASYNC_MSG_CONSUMED;
  }
}

int32_t l2_app_fib_pkt_process(struct of_app_msg *app_msg)
{
  uint32_t in_port;
  uint8_t  *pkt_data;
  struct dprm_port_entry *dp_port_entry;
  uint64_t    port_handle;
  uint8_t  sender_hw_addr[OFP_ETH_ALEN];
  uint8_t  target_hw_addr[OFP_ETH_ALEN];
  uint8_t  *ucPtr;
  uint16_t       msg_len;
  struct of_msg *reply_msg;
  int32_t  status = OF_SUCCESS;
  int32_t  retval = OF_SUCCESS;
  uint64_t  dpid;
  uint32_t  dp_index;

  uint64_t datapath_handle;
  struct of_msg *msg=NULL;
  struct ofl_packet_in  *pkt_in;

  datapath_handle=  app_msg->datapath_handle;
  dpid=app_msg->dpid;
  pkt_in=app_msg->pkt_in;
  pkt_data = app_msg->pkt_data;

  for (dp_index = 0; dp_index < no_of_l2data_paths_attached_g; dp_index++)
  {
    if (l2_dp_conf[dp_index].dp_handle == datapath_handle)
      break;
  }

  if (dp_index == no_of_l2data_paths_attached_g)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"\n%s: No datapath found with given dp handle=%llx",__FUNCTION__,datapath_handle);
    return OF_FAILURE;
  }

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"PKT-IN RECEIVED FOR L2 APP MISS ENTRY DPID=%llx dpindex=%d",dpid,dp_index);
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Received pkt of length %d",pkt_in->packet_length);

  /* Parse received packet */
  ucPtr = pkt_data;

  /* Skip Destination Mac Address */
  memcpy(target_hw_addr, ucPtr, OF_CNTLR_HW_ADDR_LEN);
  ucPtr = ucPtr + OFP_ETH_ALEN;

  /* parse sender mac address */
  memcpy(sender_hw_addr, ucPtr, OF_CNTLR_HW_ADDR_LEN);
  ucPtr += OFP_ETH_ALEN;


  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"MAC LEARN REQUEST");
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"PARSED PKT DETAILS");

  do
  {
    /* in_port = get using north bound API */

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:target_HW: %x:%x:%x:%x:%x:%x",__FUNCTION__,target_hw_addr[0],target_hw_addr[1],target_hw_addr[2],target_hw_addr[3],
        target_hw_addr[4], target_hw_addr[5]);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:sender_HW %x:%x:%x:%x:%x:%x",__FUNCTION__,sender_hw_addr[0],sender_hw_addr[1],sender_hw_addr[2],sender_hw_addr[3],
        sender_hw_addr[4], sender_hw_addr[5]);

    /* get port mac address with port id */ 
    retval = ofu_get_in_port_field(msg, pkt_in->match_fields,
        pkt_in->match_fields_length, &in_port);
    if (retval != OFU_IN_PORT_FIELD_PRESENT)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:failed get inport ....inport=%d",__FUNCTION__,in_port);
      status = OF_FAILURE;
      break;
    }
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, " PKT-IN IN PORT =%d",in_port);

    retval = get_port_handle_by_port_id(datapath_handle, in_port, &port_handle);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:failed dprm_get_port_handle port-id=%d",__FUNCTION__,in_port);
      status = OF_FAILURE;
      break;
    }

    retval = get_datapath_port_byhandle(datapath_handle, port_handle, &dp_port_entry);
    if (retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:failed get_datapath_port_byhandle =%ld",__FUNCTION__,port_handle);
      status = OF_FAILURE;
      break;
    }

    msg_len = (OFU_PACKET_OUT_MESSAGE_LEN+16+OFU_OUTPUT_ACTION_LEN);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Msg len %d",msg_len);
    /* get port mac address with port id */ 
    retval = ofu_get_in_port_field(msg, pkt_in->match_fields,
        pkt_in->match_fields_length, &in_port);
    if (retval != OFU_IN_PORT_FIELD_PRESENT)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:failed get inport ....inport=%d",__FUNCTION__,in_port);
      status = OF_FAILURE;
      break;
    }
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," PKT-IN IN PORT =%d",in_port);

    retval = get_port_handle_by_port_id(datapath_handle, in_port, &port_handle);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:failed dprm_get_port_handle port-id=%d",__FUNCTION__,in_port);
      status = OF_FAILURE;
      break;
    }

    retval = get_datapath_port_byhandle(datapath_handle, port_handle, &dp_port_entry);
    if (retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:failed get_datapath_port_byhandle =%ld",__FUNCTION__,port_handle);
      status = OF_FAILURE;
      break;
    }
    memcpy(target_hw_addr, dp_port_entry->port_info.hw_addr, OFP_ETH_ALEN);

    reply_msg = ofu_alloc_message(OFPT_PACKET_OUT, msg_len);
    if (reply_msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
      status = OF_FAILURE;
      break;
    }
  }
  while(0);

  return status;
}


int32_t
l2_app_add_mac_learn_entry(uint64_t datapath_handle,
                                 uint8_t  *sender_hw_addr, uint32_t in_port)
{
  struct of_msg *msg;
  uint16_t      msg_len;
  uint16_t      instruction_len;
  uint8_t     *inst_start_loc = NULL;
  uint16_t      match_fd_len=0;
  uint8_t     *match_start_loc = NULL;
  uint16_t      flags;
  int32_t       retval = OF_SUCCESS;
  int32_t       status = OF_SUCCESS;
  void *conn_info_p;

  msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
        OFU_ETH_SRC_MAC_ADDR_FIELD_LEN+OFU_GOTO_TABLE_INSTRUCTION_LEN);
  
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, " Msg Len %d",msg_len);

  do
  {
    msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      break;
    }

    flags = 0;
    flags |= OFPFF_RESET_COUNTS;

    retval = of_create_add_flow_entry_msg(msg,
        datapath_handle,
        L2_APP_MAC_LEARN_TABLE_ID, /*Table Id 1*/
        mac_learn_entry_priority, /*priority*/
        OFPFC_ADD,
        0, /*coockie*/
        0,/*Coockie amsk*/
        OFP_NO_BUFFER,
        flags,
        300, /*No Hard timeout*/
        0, /*Idle timeout*/
        &conn_info_p
        );

    if (retval != OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding miss entry  = %d",retval);
      status = OF_FAILURE;
    }
    ofu_start_setting_match_field_values(msg);

    retval = of_set_source_mac(msg, sender_hw_addr, FALSE, NULL);
    if(retval != OFU_SET_FIELD_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF Set IPv4 src field failed,err = %d",retval);
      status = OF_FAILURE;
      break;
    } 
  
    ofu_end_setting_match_field_values(msg, match_start_loc, &match_fd_len);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Match field len %d",match_fd_len);

    ofu_start_pushing_instructions(msg,match_fd_len);
    retval = ofu_push_go_to_table_instruction(msg, L2_APP_FIB_TABLE_ID);
    if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"go to table instruction failed err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);
    ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(instruction_len);

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"instruction len = %d", instruction_len);

    retval=of_cntrlr_send_request_message(
        msg,
        datapath_handle,
        conn_info_p,
        NULL,
        NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding mac learn entry  = %d",retval);
      status = OF_FAILURE;
    }
  }
  while(0);

  return status;
}

int32_t
l2_app_add_fib_entry(uint64_t datapath_handle,
              uint8_t  *target_hw_addr, uint32_t in_port)
{
  struct of_msg *msg;
  uint16_t      msg_len;
  uint16_t      instruction_len;
  uint8_t     *inst_start_loc = NULL;
  uint16_t      match_fd_len=0;
  uint8_t     *match_start_loc = NULL;
  uint16_t      flags;
  int32_t       retval = OF_SUCCESS;
  int32_t       status = OF_SUCCESS;
  void *conn_info_p;
  uint16_t    action_len;
  uint8_t     *action_start_loc = NULL;

  msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
        OFU_ETH_SRC_MAC_ADDR_FIELD_LEN+OFU_GOTO_TABLE_INSTRUCTION_LEN);
  
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, " Msg Len %d",msg_len);

  do
  {
    msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      break;
    }

    flags = 0;
    flags |= OFPFF_RESET_COUNTS;

    retval = of_create_add_flow_entry_msg(msg,
        datapath_handle,
        L2_APP_FIB_TABLE_ID, /*Table Id 2*/
        fib_entry_priority, /*priority*/
        OFPFC_ADD,
        0, /*coockie*/
        0,/*Coockie amsk*/
        OFP_NO_BUFFER,
        flags,
        300, /*No Hard timeout*/
        0, /*Idle timeout*/
        &conn_info_p
        );

    if (retval != OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding miss entry  = %d",retval);
      status = OF_FAILURE;
    }
    ofu_start_setting_match_field_values(msg);

    retval = of_set_destination_mac(msg,target_hw_addr, FALSE, NULL);
    if(retval != OFU_SET_FIELD_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF Set IPv4 src field failed,err = %d",retval);
      status = OF_FAILURE;
      break;
    } 
	ofu_end_setting_match_field_values(msg, match_start_loc, &match_fd_len);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Match field len %d",match_fd_len);
	
#if 0  
    
    ofu_start_pushing_instructions(msg,match_fd_len);

	
    retval = ofu_push_go_to_table_instruction(msg, L2_APP_FIB_TABLE_ID);
    if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"go to table instruction failed err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);
    ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(instruction_len);
#else
    ofu_start_pushing_instructions(msg, match_fd_len);
    retval = ofu_start_pushing_apply_action_instruction(msg);
    if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding apply action instruction err = %d",retval);
      status = OF_FAILURE;
      break;
    }

    retval = ofu_push_output_action(msg, in_port, OFPCML_NO_BUFFER);
    if (retval != OFU_ACTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding output action  err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_pushing_actions(msg,action_start_loc,&action_len);
    ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(action_len);
    ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);
#endif

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"instruction len = %d", instruction_len);

    retval=of_cntrlr_send_request_message(
        msg,
        datapath_handle,
        conn_info_p,
        NULL,
        NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding FIB entry  = %d",retval);
      status = OF_FAILURE;
    }
  }
  while(0);

  return status;
}
