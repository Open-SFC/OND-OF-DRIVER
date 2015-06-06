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
 * File name: cntrucmapp.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT

#include"cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"

#define CPDPCOMM_CM_APPID      IGWCPDP_DP_UCM
#define CPDPCOMM_CM_SUBAPPID   IGWCPDP_DP_UCM


void cntrl_ucm_process_request ( struct sockaddr_in *peer_address, ucmcntrlmsg_t *Msg);


struct cm_channel_applcallback *cntrlr_ucm_app_head=NULL;
struct sockaddr_in peer_address_g={};
uint8_t peeraddr_valid_g;

int32_t ucm_cntrl_app_init(void)
{

  cntrlrucm_debugmsg("ucm_cntrl_app_init Entry");



  of_switch_appl_ucmcbk_init();
  of_domain_appl_ucmcbk_init();
  //dprm_table_appl_ucmcbk_init ();
  datapath_appl_ucmcbk_init ();
  dprm_datapath_port_appl_ucmcbk_init();
  dprm_datapath_dpattribute_appl_ucmcbk_init();
  dprm_switch_attribute_appl_ucmcbk_init();
  dprm_domain_attribute_appl_ucmcbk_init();	
  dprm_datapath_port_attribute_appl_ucmcbk_init();	
  dprm_datapath_channels_appl_cmcbk_init();
  dprm_datapath_auxchannels_appl_cmcbk_init();
         of_swinfo_appl_cmcbk_init();
  of_mempoolstats_appl_ucmcbk_init ();
  of_portstats_appl_ucmcbk_init ();
  of_tablestats_appl_ucmcbk_init();   
  of_flowstats_appl_ucmcbk_init();
  of_bindstats_appl_ucmcbk_init();
  //ipsec_policy_stats_appl_ucmcbk_init();
  of_aggrstats_appl_ucmcbk_init();
  of_ipopts_appl_ucmcbk_init();  
	of_cntlrrole_appl_ucmcbk_init();
of_asyncmsgcfg_appl_ucmcbk_init();	
  of_tblfeatures_appl_ucmcbk_init();
  of_group_appl_ucmcbk_init();
  of_bucket_appl_ucmcbk_init();
  of_action_appl_ucmcbk_init();
  of_group_stats_appl_ucmcbk_init();
  of_group_features_appl_ucmcbk_init();
  of_meter_appl_ucmcbk_init();
  of_meter_band_appl_ucmcbk_init();
  of_meter_stats_appl_ucmcbk_init();
  of_meter_features_appl_ucmcbk_init();

  of_ssl_appl_ucmcbk_init();
  of_event_appl_ucmcbk_init();
  
  of_ssl_appl_addnlcacerts_ucmcbk_init();
  of_tracelog_appl_ucmcbk_init();

  #if 1
  of_flow_appl_ucmcbk_init();
  of_flow_match_appl_ucmcbk_init();
  of_flow_instruction_appl_ucmcbk_init();
  of_flow_actionlist_appl_ucmcbk_init();
  of_flow_actionset_appl_ucmcbk_init();
  #endif

 of_view_appl_ucmcbk_init ();
 of_namespace_appl_ucmcbk_init ();

//#ifdef CNTRL_NETWORK_ELEMENT_MAPPER_SUPPORT
#if 0 
 of_nem_ns2nsid_appl_ucmcbk_init();
 of_nem_nsid2ns_appl_ucmcbk_init();
 of_nem_dpid2nsid_appl_ucmcbk_init();
 of_nem_nsid2dpid_appl_ucmcbk_init();
 of_nem_dpid2peth_appl_ucmcbk_init();
 of_nem_peth2dpid_appl_ucmcbk_init();
#endif /* CNTRL_NETWORK_ELEMENT_MAPPER_SUPPORT */

  crm_tenant_appl_ucmcbk_init();//tenant init
  crm_tenant_attribute_appl_ucmcbk_init();
  crm_vm_appl_ucmcbk_init();
  crm_vm_attribute_appl_ucmcbk_init();
  crm_subnet_appl_ucmcbk_init();//subnt init
  crm_subnet_attribute_appl_ucmcbk_init();
  crm_vn_appl_ucmcbk_init();
  crm_vn_compuetnode_appl_ucmcbk_init();
  crm_vm_sideports_appl_ucmcbk_init();
  crm_port_attribute_appl_ucmcbk_init();
  crm_vn_attribute_appl_ucmcbk_init();
  crm_vn_nw_sideports_appl_ucmcbk_init();
  crm_nwport_attribute_appl_ucmcbk_init(); 

  of_protocol_appl_ucmcbk_init();//protocol init
  #if 1
  nsrm_appliance_init(); 
  nsrm_nwservice_attribute_appl_ucmcbk_init();
  nsrm_chain_init();
  nsrm_nschain_attribute_appl_ucmcbk_init();
  nsrm_attach_nwservice_to_nschain_init();
  nsrm_nwservice_instance_init();
  nsrm_chainset_init();
  nsrm_nschainset_attribute_appl_ucmcbk_init();
  nsrm_l2networkmap_init();
  nsrm_l2nw_map_attribute_appl_ucmcbk_init();
  nsrm_chainselrule_init();
  nsrm_bypassrule_init();
  nsrm_bypass_rule_attribute_appl_ucmcbk_init();
  nsrm_selection_rule_attribute_appl_ucmcbk_init();
  nsrm_attach_nwservice_to_bypass_rule_init();
  #endif

  of_ttp_appl_init();
  of_ttp_tbl_appl_init();
  of_ttp_domain_appl_init();
  of_ttp_match_appl_init();

  return OF_SUCCESS;
}

int32_t cm_register_app_callbacks(uint32_t ApplicationId,struct cm_dm_app_callbacks *cm_app_cbk_p)
{
  struct cm_channel_applcallback *app_node;

  cntrlrucm_debugmsg("ApplId=%d::",ApplicationId);
  app_node=cntrlr_ucm_app_head;
  while(app_node)
  {
    if(app_node->appl_id_ui==ApplicationId)
      break;
    app_node=app_node->next_p;
  }

  if(app_node)
    return OF_FAILURE;

  app_node=of_calloc(1,sizeof(struct cm_channel_applcallback));

  if(app_node==NULL)
    return OF_FAILURE;

  app_node->appl_id_ui=ApplicationId;
  app_node->app_call_backs.cm_start_xtn=cm_app_cbk_p->cm_start_xtn;
  app_node->app_call_backs.cm_add_rec=cm_app_cbk_p->cm_add_rec;
  app_node->app_call_backs.cm_modify_rec=cm_app_cbk_p->cm_modify_rec;
  app_node->app_call_backs.cm_delete_rec=cm_app_cbk_p->cm_delete_rec;
  app_node->app_call_backs.cm_end_xtn=cm_app_cbk_p->cm_end_xtn;
  app_node->app_call_backs.cm_getfirst_nrecs=cm_app_cbk_p->cm_getfirst_nrecs;
  app_node->app_call_backs.cm_getnext_nrecs=cm_app_cbk_p->cm_getnext_nrecs;
  app_node->app_call_backs.cm_getexact_rec=cm_app_cbk_p->cm_getexact_rec;
  app_node->app_call_backs.cm_test_config=cm_app_cbk_p->cm_test_config;
  app_node->next_p=cntrlr_ucm_app_head;

  cntrlr_ucm_app_head=app_node;

  cntrlrucm_debugmsg("ApplId=%d::SUCCESS",ApplicationId);
  return OF_SUCCESS;
}

struct cm_dm_app_callbacks *UCMDPGetApplCallbackNode(uint32_t app_id)
{
  struct cm_channel_applcallback *app_node;
  cntrlrucm_debugmsg("Entry::ApplId=%d",app_id);

  app_node=cntrlr_ucm_app_head;

  while(app_node)
  {
    if(app_node->appl_id_ui==app_id)
      break;
    app_node=app_node->next_p;
  }

  if(app_node)
    return &app_node->app_call_backs;

  cntrlrucm_debugmsg("no callbacks found for ApplId=%d",app_id);

  return NULL;
}

void cntrl_ucm_process_request (struct sockaddr_in *peer_address, ucmcntrlmsg_t  *ucm_request)
{
  void *config_trans_p=NULL;
  uint32_t length=0,total_length=0, trans_len=0;
  uint32_t app_id,trans_cmd;
  int32_t return_value = 0,cmd_id,sub_cmd_id,result=0,ii=0;
  char *msg_buf;
  uint32_t no_of_records=0,i,rec_cnt;

  ucmcntrlmsg_t *cntrl_response=NULL;
  struct cm_dm_app_callbacks *cm_app_cbk_p=NULL;
  struct cm_app_result *pResult = NULL;
  struct cm_array_of_iv_pairs mand_iv_pairs={};
  struct cm_array_of_iv_pairs opt_iv_pairs={};
  struct cm_array_of_iv_pairs trans_iv_pairs={};
  struct cm_array_of_iv_pairs key_iv_pairs={};
  struct cm_array_of_iv_pairs *result_iv_pairs_p=NULL;
  struct cm_array_of_iv_pairs  prev_iv_pairs={};
  char *c;

  cntrlrucm_debugmsg("Entry::sizeof ucmcntrlmsg_t is %d",sizeof(ucmcntrlmsg_t));
#if 0
  c= (char *)ucm_request;
  CM_CNTRL_PRINT_PACKET(c,100);
#endif
  app_id = ucm_request->app_id;
  cmd_id =  ucm_request->sub_cmd_id;
  trans_cmd=ucm_request->cmd_id;
  trans_len=ucm_request->trans_len;
  cntrlrucm_debugmsg("msg leng=%d  AplId=%d::icmd=%d subcmd=%d",ucm_request->length,ucm_request->app_id,ucm_request->cmd_id, ucm_request->sub_cmd_id);

  cm_app_cbk_p=UCMDPGetApplCallbackNode(ucm_request->app_id);

  cntrl_response=(ucmcntrlmsg_t *)of_calloc(1,sizeof(ucmcntrlmsg_t));
  if(cntrl_response == NULL)
  {
    cntrl_ucm_sendresponse(peer_address,(char *)cntrl_response,sizeof(ucmcntrlmsg_t));
    return;
  }

  msg_buf=ucm_request->msg;
  cntrl_response->app_id=ucm_request->app_id;
  cntrl_response->msg_type=CM_CNTRL_CONFIG_RESPONSE;
  cntrl_response->cmd_id=ucm_request->cmd_id;
  cntrl_response->sub_cmd_id=ucm_request->sub_cmd_id;
  cntrl_response->result_code=OF_SUCCESS;
  cntrl_response->error_code=OF_SUCCESS;
  cntrl_response->trans_len=0;
  cntrl_response->length=0;

  if(cm_app_cbk_p==NULL)
  {
    cntrlrucm_debugmsg("App CallBacks are null");
    cntrl_response->result_code=OF_FAILURE;
    cntrl_response->error_code=CM_CPDP_APPL_NOT_REG;
    cntrl_ucm_sendresponse(peer_address,(char *)cntrl_response,sizeof(ucmcntrlmsg_t));
    return;
  }
  switch(cmd_id)
  {
    case CM_CPDP_APPL_TRANS_BEGIN_CMD:
      {
        cntrlrucm_debugmsg("Appl Trans Command");        
        do
        {
          if(cm_app_cbk_p->cm_start_xtn==NULL)
          {
            cntrlrucm_debugmsg("DPCHANNEL   cm_start_xtn  is NULL");        
            cntrl_response->result_code=OF_FAILURE;
            cntrl_response->error_code=CM_NO_APPL_TRANS_SUPPORT;
            break;
          }
          length=0;
          if(UCMDPJECopyIvPairArryFromBuffer(msg_buf,&trans_iv_pairs,&length)!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg(" UCMDPJECopyIvPairArryFromBuffer Error");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_START_TRANS_FAILED;
            break;
          }
          msg_buf+=length;
          cntrlrucm_debugmsg("TransParam length = %d",length);
          config_trans_p=cm_app_cbk_p->cm_start_xtn(&trans_iv_pairs,trans_cmd, &pResult);
          if(config_trans_p==NULL)
          {
            cntrlrucm_debugmsg("DPCHANNEL   cm_start_xtn  Failed");        
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_START_TRANS_FAILED;
            if(pResult && pResult->result_string_p)
            {
              cntrl_response->length = of_strlen(pResult->result_string_p);
              of_memcpy(cntrl_response->msg, pResult->result_string_p,cntrl_response->length);
            }
            break;
          }
          msg_buf=cntrl_response->msg;
          total_length=0;
          cntrl_response->trans_len=sizeof(long);
#if defined(_LP64)
	 msg_buf= of_mbuf_put_64(msg_buf, config_trans_p);
#else
	 msg_buf= of_mbuf_put_32(msg_buf, config_trans_p);
#endif

          total_length+=sizeof(long);
          msg_buf+=sizeof(long);
        }while(0);
        cntrl_response->length=total_length;
        cntrl_ucm_sendresponse(peer_address,(char *)cntrl_response,sizeof(ucmcntrlmsg_t));
        of_free(cntrl_response);
        UCMDPJEFreeIvPairArray(&trans_iv_pairs);
        return;
      }
      break;

    case CM_CPDP_APPL_ADD_CMD:
      {
        cntrlrucm_debugmsg("Add Command");        
        do 
        {
          if(cm_app_cbk_p->cm_add_rec==NULL)
          {
            cntrlrucm_debugmsg("DPCHANNEL   cm_add_rec  is NULL");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code=CM_APPL_NULL_CALLBACK;
            break;
          } 
#if defined(_LP64)
	 config_trans_p = (void *)of_mbuf_get_64(msg_buf);
#else
	 config_trans_p = (void *)of_mbuf_get_32(msg_buf);
#endif
          msg_buf+=trans_len;
          length = 0;
          if(UCMDPJECopyIvPairArryFromBuffer(msg_buf,&mand_iv_pairs,&length)!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg(" UCMDPJECopyIvPairArryFromBuffer Error");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->length = of_strlen("Copy IV pair failed");
            of_strcpy(cntrl_response->msg,"Copy IV pair failed");
            break;
          }  
          msg_buf+=length;
          cntrlrucm_debugmsg("Mesg length = %d",length);
          length=0;
          if(UCMDPJECopyIvPairArryFromBuffer(msg_buf,&opt_iv_pairs,&length)!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg(" UCMDPJECopyIvPairArryFromBuffer Error");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code=CM_APPL_ADD_FAILED;
            break;
          }
          result=cm_app_cbk_p->cm_add_rec(config_trans_p,&mand_iv_pairs,&opt_iv_pairs,&pResult);
          if(result!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg("DPCHANNEL   cm_add_rec  Failed");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code=CM_APPL_ADD_FAILED;
            if(pResult && pResult->result_string_p)
            {
              cntrl_response->length = of_strlen(pResult->result_string_p);
              of_memcpy(cntrl_response->msg, pResult->result_string_p,cntrl_response->length);
            }
            break;
          }
        }while(0);
        cntrl_ucm_sendresponse(peer_address,(char *)cntrl_response,sizeof(ucmcntrlmsg_t));
        of_free(cntrl_response);
        UCMDPJEFreeIvPairArray(&mand_iv_pairs);
        UCMDPJEFreeIvPairArray(&opt_iv_pairs);
        return;
      }
      break;
    case CM_CPDP_APPL_SET_CMD:
      {
        cntrlrucm_debugmsg("Set Command");        
        do
        {
          if(cm_app_cbk_p->cm_modify_rec==NULL)
          {
            cntrlrucm_debugmsg("DPCHANNEL   cm_modify_rec  is NULL");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_NULL_CALLBACK;
            break;
          }
#if defined(_LP64)
	 config_trans_p = (void *)of_mbuf_get_64(msg_buf);
#else
	 config_trans_p = (void *)of_mbuf_get_32(msg_buf);
#endif
          msg_buf+=trans_len;
          length =0;
          if(UCMDPJECopyIvPairArryFromBuffer(msg_buf,&mand_iv_pairs,&length)!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg("UCMDPJECopyIvPairArryFromBuffer Error");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_SET_FAILED;
            break;
          } 
          msg_buf+=length;
          length=0;
          if(UCMDPJECopyIvPairArryFromBuffer(msg_buf,&opt_iv_pairs,&length)!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg(" UCMDPJECopyIvPairArryFromBuffer Error");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->length = of_strlen("Copy Optional IV pair failed");
            of_strcpy(cntrl_response->msg,"Copy Optional IV pair failed");
            break;
          }
          cntrlrucm_debugmsg(" OptP length = %d",length);
          result=cm_app_cbk_p->cm_modify_rec(config_trans_p,&mand_iv_pairs,&opt_iv_pairs,&pResult);
          if(result!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg("DPCHANNEL   UCMConfigModRecord  Failed");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_SET_FAILED;
            if(pResult && pResult->result_string_p)
            {
              cntrl_response->length = of_strlen(pResult->result_string_p);
              of_memcpy(cntrl_response->msg, pResult->result_string_p,cntrl_response->length);
            }
            break;
          }
        }while(0);
        cntrl_ucm_sendresponse(peer_address,(char *)cntrl_response,sizeof(ucmcntrlmsg_t));
        of_free(cntrl_response);
        UCMDPJEFreeIvPairArray(&mand_iv_pairs);
        UCMDPJEFreeIvPairArray(&opt_iv_pairs);
        return;
      }
      break;

    case CM_CPDP_APPL_DEL_CMD:
      {
        cntrlrucm_debugmsg("Del Command");        
        do
        {
          if(cm_app_cbk_p->cm_delete_rec==NULL)
          {
            cntrlrucm_debugmsg("DPCHANNEL   cm_delete_rec  is NULL");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_NULL_CALLBACK;
            break;
          }
#if defined(_LP64)
	 config_trans_p = (void *)of_mbuf_get_64(msg_buf);
#else
	 config_trans_p = (void *)of_mbuf_get_32(msg_buf);
#endif
          msg_buf+=trans_len;
          length =0;
          if(UCMDPJECopyIvPairArryFromBuffer(msg_buf,&mand_iv_pairs,&length)!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg(" UCMDPJECopyIvPairArryFromBuffer Error");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_DEL_FAILED;
            break;
          }  
          msg_buf+=length;
          cntrlrucm_debugmsg(" Del cmd length = %d",length);
          result=cm_app_cbk_p->cm_delete_rec(config_trans_p,&mand_iv_pairs,&pResult);
          if(result!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg("DPCHANNEL   cm_delete_rec  Failed");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_DEL_FAILED;
            if(pResult && pResult->result_string_p)
            {
              cntrl_response->length = of_strlen(pResult->result_string_p);
              of_memcpy(cntrl_response->msg, pResult->result_string_p,cntrl_response->length);
              break;
            }
          }
        }while(0);

        cntrl_ucm_sendresponse(peer_address,(char *)cntrl_response,sizeof(ucmcntrlmsg_t));
        of_free(cntrl_response);
        UCMDPJEFreeIvPairArray(&mand_iv_pairs);
        return;
      }
      break;

    case CM_CPDP_APPL_TRANS_END_CMD:
      {
        cntrlrucm_debugmsg("End Trans Command");        
        do
        {
          if(cm_app_cbk_p->cm_end_xtn==NULL) 
          {
            cntrlrucm_debugmsg("DPCHANNEL   cm_end_xtn  is NULL");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_NULL_CALLBACK;
            break;
          }
#if defined(_LP64)
	 config_trans_p = (void *)of_mbuf_get_64(msg_buf);
#else
	 config_trans_p = (void *)of_mbuf_get_32(msg_buf);
#endif
          msg_buf+=trans_len;
          length = 0;
          cntrlrucm_debugmsg(" End Trans trans_cmd = %d",trans_cmd);
          result=cm_app_cbk_p->cm_end_xtn(config_trans_p,trans_cmd, &pResult);
          if(result!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg("DPCHANNEL   cm_end_xtn  Failed");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_END_TRANS_FAILED;
            if(pResult && pResult->result_string_p)
            {
              cntrl_response->length = of_strlen(pResult->result_string_p);
              of_memcpy(cntrl_response->msg, pResult->result_string_p,cntrl_response->length);
            }
            break;
          }
        }while(0);
        cntrl_ucm_sendresponse(peer_address,(char *)cntrl_response,sizeof(ucmcntrlmsg_t));
        of_free(cntrl_response);
        return;
      }
      break;

    case CM_CPDP_APPL_GET_FIRSTN_CMD:
      {
        cntrlrucm_debugmsg("Get First Command");        
        do
        {
          cntrlrucm_debugmsg(" CM_CPDP_APPL_GET_FIRSTN_CMD: Entry ");
          if(cm_app_cbk_p->cm_getfirst_nrecs==NULL) 
          {
            cntrlrucm_debugmsg("DPCHANNEL   cm_getfirst_nrecs  is NULL");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_NULL_CALLBACK;
            break;
          }
          length=0;
          if(UCMDPJECopyIvPairArryFromBuffer(msg_buf,&key_iv_pairs,&length)!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg(" UCMDPJECopyIvPairArryFromBuffer Error");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_GETFIRST_NREC_FAILED;
            break;
          }  
          msg_buf+=length;
          no_of_records = of_mbuf_get_32 (msg_buf);
          msg_buf+=4;
          result_iv_pairs_p=NULL;
          result=cm_app_cbk_p->cm_getfirst_nrecs(&key_iv_pairs,&no_of_records,&result_iv_pairs_p);
          if (result == CM_CNTRL_MULTIPART_RESPONSE)
          {
            cntrlrucm_debugmsg("Multipart resposen will be sent");
            cntrl_response->msg_type=CM_CNTRL_MULTIPART_RESPONSE;
            cntrl_response->multi_part=CM_CNTRL_MULTIPART_REPLY_MORE;
            of_memcpy(&peer_address_g,peer_address,sizeof(struct sockaddr_in));
            peeraddr_valid_g=TRUE;
            break;
          }
          if(result!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg("DPCHANNEL   cm_getfirst_nrecs  Failed");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_GETFIRST_NREC_FAILED;
            break;
          }
          msg_buf=cntrl_response->msg;
          total_length=0;
          of_mbuf_put_32(msg_buf,no_of_records); 
          total_length+=4;
          msg_buf+=4;
          for(ii=0;ii<no_of_records;ii++)
          {
            length=0;
            UCMDPJECopyIvPairArrayToBuffer(&result_iv_pairs_p[ii],msg_buf,&length);
            msg_buf+=length;
            total_length+=length;
          }
        }
        while(0);
        cntrl_response->length=total_length;
        cntrl_ucm_sendresponse(peer_address,(char *)cntrl_response,sizeof(ucmcntrlmsg_t));
        of_free(cntrl_response);
        UCMDPJEFreeIvPairArray (&key_iv_pairs);
        if ((result == OF_SUCCESS) && result_iv_pairs_p)
        {
          for(rec_cnt=0; rec_cnt < no_of_records; rec_cnt++)
          {
            UCMDPJEFreeIvPairArray (&result_iv_pairs_p[rec_cnt]);
          }
          of_free(result_iv_pairs_p);
        }
        return;
      }
      break;

    case CM_CPDP_APPL_GET_NEXTN_CMD:
      {
        cntrlrucm_debugmsg("Get Next Command");        
        do
        {
          if(cm_app_cbk_p->cm_getnext_nrecs==NULL) 
          {
            cntrlrucm_debugmsg("DPCHANNEL   cm_getnext_nrecs  is NULL");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_NULL_CALLBACK;
            break;
          }
          length=0;
          if(UCMDPJECopyIvPairArryFromBuffer(msg_buf,&key_iv_pairs,&length)!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg(" UCMDPJECopyIvPairArryFromBuffer Error");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_GETNEXT_NREC_FAILED;
            break;
          }  
          msg_buf+=length;
          length=0;
          if(UCMDPJECopyIvPairArryFromBuffer(msg_buf,&prev_iv_pairs,&length)!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg(" UCMDPJECopyIvPairArryFromBuffer Error");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_GETNEXT_NREC_FAILED;
            break;
          }  
          msg_buf+=length;
          no_of_records = of_mbuf_get_32 (msg_buf);
          msg_buf+=4;
          result_iv_pairs_p=NULL;
          result=cm_app_cbk_p->cm_getnext_nrecs(&key_iv_pairs,&prev_iv_pairs,&no_of_records,&result_iv_pairs_p);
          if(result!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg("DPCHANNEL   cm_getnext_nrecs  Failed");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_GETNEXT_NREC_FAILED;
            break;
          }
          msg_buf=cntrl_response->msg;
          total_length=0;
          of_mbuf_put_32(msg_buf, no_of_records); 
          total_length+=4;
          msg_buf+=4;

          for(ii=0;ii<no_of_records;ii++)
          {
            length=0;
            UCMDPJECopyIvPairArrayToBuffer(&result_iv_pairs_p[ii],msg_buf,&length);
            msg_buf+=length;
            total_length+=length;
          }
          cntrl_response->length=total_length;
        }while(0);
        cntrl_ucm_sendresponse(peer_address,(char *)cntrl_response,sizeof(ucmcntrlmsg_t));
        of_free(cntrl_response);
        UCMDPJEFreeIvPairArray (&prev_iv_pairs);
        UCMDPJEFreeIvPairArray (&key_iv_pairs);
        if ((result == OF_SUCCESS) && result_iv_pairs_p)
        {
          for(rec_cnt=0; rec_cnt < no_of_records; rec_cnt++)
          {
            UCMDPJEFreeIvPairArray (&result_iv_pairs_p[rec_cnt]);
          }
          of_free(result_iv_pairs_p);
        }
      }
      break;

    case CM_CPDP_APPL_GET_EACT_CMD:
      {
        cntrlrucm_debugmsg("Get Exact Command");        
        do
        {
          if(cm_app_cbk_p->cm_getexact_rec==NULL) 
          {
            cntrlrucm_debugmsg("DPCHANNEL cm_getexact_rec  is NULL");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_NULL_CALLBACK;
            break;
          }
          length=0;
          if(UCMDPJECopyIvPairArryFromBuffer(msg_buf,&key_iv_pairs,&length)!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg(" UCMDPJECopyIvPairArryFromBuffer Error");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_GETEXACT_REC_FAILED;
            break;
          }  
          msg_buf+=length;
          result_iv_pairs_p=NULL;
          result=cm_app_cbk_p->cm_getexact_rec(&key_iv_pairs,&result_iv_pairs_p);
          if (result == CM_CNTRL_MULTIPART_RESPONSE)
          {
            cntrlrucm_debugmsg("Multipart resposen will be sent");
            cntrl_response->msg_type=CM_CNTRL_MULTIPART_RESPONSE;
            cntrl_response->multi_part=CM_CNTRL_MULTIPART_REPLY_MORE;
            of_memcpy(&peer_address_g,peer_address,sizeof(struct sockaddr_in));
            peeraddr_valid_g=TRUE;
            break;
          }
          if(result!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg("DPCHANNEL   cm_getexact_rec  Failed");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_GETEXACT_REC_FAILED;
            break;
          }
          total_length=0;
          msg_buf=cntrl_response->msg;
          no_of_records=1;
          length=0;
          UCMDPJECopyIvPairArrayToBuffer(result_iv_pairs_p,msg_buf,&length);
          msg_buf+=length;
          total_length+=length;
          cntrl_response->length=total_length;
        }while(0);

        cntrl_ucm_sendresponse(peer_address,(char *)cntrl_response,sizeof(ucmcntrlmsg_t));
        of_free(cntrl_response);
        UCMDPJEFreeIvPairArray (&key_iv_pairs);
        if ((result == OF_SUCCESS) && result_iv_pairs_p)
        {
          for(rec_cnt=0; rec_cnt < no_of_records; rec_cnt++)
          {
            UCMDPJEFreeIvPairArray (&result_iv_pairs_p[rec_cnt]);
          }
          of_free(result_iv_pairs_p);
        }
      }
      break;

    case CM_CPDP_APPL_TEST_CMD:
      {
        cntrlrucm_debugmsg("Verify Command");        
        do
        {
          if(cm_app_cbk_p->cm_test_config==NULL) 
          {
            cntrlrucm_debugmsg("DPCHANNEL  cm_test_config  is NULL");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_NULL_CALLBACK;
            break;
          }
          length=0;
          if(UCMDPJECopyIvPairArryFromBuffer(msg_buf,&key_iv_pairs,&length)!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg(" UCMDPJECopyIvPairArryFromBuffer Error");
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_TEST_FAILED;
            break;
          }
          msg_buf+=length;

          result=cm_app_cbk_p->cm_test_config(&key_iv_pairs,trans_cmd,&pResult);
          if(result!=OF_SUCCESS)
          {
            cntrlrucm_debugmsg("DPCHANNEL   cm_delete_rec  Failed");
            cntrl_response->result_code = OF_FAILURE;
            if(pResult && pResult->result_string_p)
            {
              cntrl_response->length = of_strlen(pResult->result_string_p);
              of_memcpy(cntrl_response->msg, pResult->result_string_p,cntrl_response->length);
            }
            cntrl_response->result_code = OF_FAILURE;
            cntrl_response->error_code = CM_APPL_TEST_FAILED;
            break;
          }

        }while(0);

        cntrl_ucm_sendresponse(peer_address,(char *)cntrl_response,sizeof(ucmcntrlmsg_t));
        of_free(cntrl_response);
        UCMDPJEFreeIvPairArray(&key_iv_pairs);
        return;
      }
      break;

    default:
      cntrl_response->result_code = OF_FAILURE;
      cntrl_response->error_code = CM_APPL_INVALID_CMD;
      cntrl_ucm_sendresponse(peer_address,(char *)cntrl_response,sizeof(ucmcntrlmsg_t));
      of_free(cntrl_response);
      return;
  }
  return;
}
void cntrl_ucm_async_msg_cfg_response(struct ofp_async_config   *of_async_config_msg)
{
	struct cm_array_of_iv_pairs *result_iv_pairs_p=NULL;
	uint32_t  rec_index=0;
	result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof(struct cm_array_of_iv_pairs));
	if (result_iv_pairs_p == NULL)
	{
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "failed to allocate memory\r\n",__FUNCTION__,__LINE__);
		return;
	}
	dprm_datapath_asyncmsgcfg_ucm_getparms(of_async_config_msg, &result_iv_pairs_p[rec_index]);
	return;
}

void cntrl_ucm_send_multipart_response(uint32_t multipart_type,void *data_p, uint8_t last)
{
  uint32_t app_id;
  ucmcntrlmsg_t *cntrl_response;
  uint32_t total_length, no_of_records=1, length, rec_index=0,ii;
  char *msg_buf;
  struct cm_array_of_iv_pairs *result_iv_pairs_p=NULL;


  cntrlrucm_debugmsg("Entry multipart type=%d",multipart_type);
  cntrl_response=of_calloc(1,sizeof(ucmcntrlmsg_t));
  cntrl_response->msg_type=CM_CNTRL_MULTIPART_RESPONSE;
  if (last == TRUE)
    cntrl_response->multi_part=0; /* no more replies */
  else
    cntrl_response->multi_part=CM_CNTRL_MULTIPART_REPLY_MORE;
  cntrl_response->cmd_id=0;
  cntrl_response->sub_cmd_id=CM_CPDP_APPL_GET_FIRSTN_CMD;
  cntrl_response->result_code=OF_SUCCESS;
  cntrl_response->error_code=OF_SUCCESS;
  cntrl_response->trans_len=0;
  cntrl_response->length=0;
  msg_buf=cntrl_response->msg;

  switch(multipart_type)
  {
    case OFPMP_PORT_STATS:
      cntrl_response->app_id=CM_ON_DIRECTOR_DATAPATH_STATS_PORTSTATS_APPL_ID;
      break;
    case OFPMP_TABLE:
      cntrl_response->app_id=CM_ON_DIRECTOR_DATAPATH_STATS_TABLESTATS_APPL_ID;
      break;
    case OFPMP_FLOW:
      cntrl_response->app_id=CM_ON_DIRECTOR_DATAPATH_STATS_FLOWSTATS_APPL_ID;
      break;
    case OFPMP_AGGREGATE:
      cntrl_response->app_id=CM_ON_DIRECTOR_DATAPATH_STATS_AGGREGATESTATS_APPL_ID;
      break;
    case OFPMP_GROUP_DESC:
      cntrl_response->app_id=CM_ON_DIRECTOR_DATAPATH_GROUPS_GROUP_APPL_ID;
      break;
    case OFPMP_GROUP:
      cntrl_response->app_id=CM_ON_DIRECTOR_DATAPATH_GROUPS_STATS_APPL_ID;
      break;
    case OFPMP_GROUP_FEATURES:
      cntrl_response->app_id=CM_ON_DIRECTOR_DATAPATH_GROUPS_FEATURES_APPL_ID;
      break;
    case OFPMP_TABLE_FEATURES:
      cntrl_response->app_id=CM_ON_DIRECTOR_DATAPATH_STATS_TABLEFEATURES_APPL_ID;
      break;
    case OFPMP_IPOPTS_DESC:
      cntrl_response->app_id=CM_ON_DIRECTOR_DATAPATH_IPOPTIONS_APPL_ID;
      break; 
    case OFPMP_DESC:
      cntrl_response->app_id=CM_ON_DIRECTOR_DATAPATH_SWITCHINFO_APPL_ID;
      break;
    case OFPMP_METER_CONFIG:
      cntrl_response->app_id=CM_ON_DIRECTOR_DATAPATH_METERS_METER_APPL_ID;
      break;
    case OFPMP_METER_FEATURES:
      cntrl_response->app_id=CM_ON_DIRECTOR_DATAPATH_METERS_FEATURES_APPL_ID;
      break;
    case OFPMP_METER:
      cntrl_response->app_id=CM_ON_DIRECTOR_DATAPATH_METERS_STATS_APPL_ID;
      break;
    case OFPMP_EXPERIMENTER:
      cntrl_response->app_id=CM_ON_DIRECTOR_DATAPATH_STATS_BINDSTATS_APPL_ID;
      break;
    default:
      cntrlrucm_debugmsg("unhandled multipart_type=%d",multipart_type);
      cntrl_response->result_code=OF_FAILURE;
      cntrl_response->multi_part=0;
      goto send_reply;
  }

  if(data_p == NULL)
  {
    cntrl_response->result_code=OF_FAILURE;
    cntrl_response->multi_part=0;
    goto send_reply;
  }

  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    cntrlrucm_debugmsg("memory allocation failed");
    return;
  }

  switch(cntrl_response->app_id)
  {
    case CM_ON_DIRECTOR_DATAPATH_STATS_PORTSTATS_APPL_ID:
      {
        struct ofi_port_stats_info *port_stats= (struct ofi_port_stats_info*)data_p;
        //      cntrlrucm_debugmsg("port stats multipart response");
        of_portstats_ucm_getparams (port_stats, &result_iv_pairs_p[rec_index]);
      }
      break;
    case CM_ON_DIRECTOR_DATAPATH_STATS_TABLESTATS_APPL_ID:
      {
        struct ofi_table_stats_info *table_stats= (struct ofi_table_stats_info*)data_p;
        //        cntrlrucm_debugmsg("table stats multipart response");
        of_tablestats_ucm_getparams (table_stats, &result_iv_pairs_p[rec_index]);
      }
      break;
    case CM_ON_DIRECTOR_DATAPATH_STATS_FLOWSTATS_APPL_ID:
      {
        struct ofi_flow_entry_info *flow_stats= (struct ofi_flow_entry_info*)data_p;
      //  cntrlrucm_debugmsg("flow stats multipart response");
        of_flowstats_ucm_getparams (flow_stats, &result_iv_pairs_p[rec_index]);
      }
      break;
    case CM_ON_DIRECTOR_DATAPATH_STATS_AGGREGATESTATS_APPL_ID:
      {
        struct ofi_aggregate_flow_stats *aggr_stats= (struct ofi_aggregate_flow_stats*)data_p;
        //      cntrlrucm_debugmsg("aggr stats multipart response");
        of_aggrstats_ucm_getparams (aggr_stats, &result_iv_pairs_p[rec_index]);
      }
      break;
    case CM_ON_DIRECTOR_DATAPATH_GROUPS_GROUP_APPL_ID:
      {
        struct ofi_group_desc_info *group_info=(struct ofi_group_desc_info *)data_p;
        //      cntrlrucm_debugmsg("group desc info");
        of_group_ucm_getparams(group_info, &result_iv_pairs_p[rec_index]);
      }
      break;
    case CM_ON_DIRECTOR_DATAPATH_GROUPS_STATS_APPL_ID:
      {
        struct ofi_group_stat *group_info=(struct ofi_group_stat *)data_p;
        //      cntrlrucm_debugmsg("group desc info");
        of_group_stats_ucm_getparams(group_info, &result_iv_pairs_p[rec_index]);
      }
      break;
    case CM_ON_DIRECTOR_DATAPATH_GROUPS_FEATURES_APPL_ID:
      {
        struct ofi_group_features_info *group_info=(struct ofi_group_features_info *)data_p;
        //      cntrlrucm_debugmsg("group desc info");
        of_group_features_ucm_getparams(group_info, &result_iv_pairs_p[rec_index]);
      }
      break;
    case CM_ON_DIRECTOR_DATAPATH_STATS_TABLEFEATURES_APPL_ID:
      {
        struct ofi_table_features_info *table_features= (struct ofi_table_features_info *)data_p;
        //cntrlrucm_debugmsg("%s(%d)::table features multipart response",__FUNCTION__,__LINE__);
        of_tblfeatures_ucm_getparams (table_features, &result_iv_pairs_p[rec_index]);
      }
      break;

    case CM_ON_DIRECTOR_DATAPATH_IPOPTIONS_APPL_ID:
      {
        struct ofl_switch_config *of_ipopts_info = (struct ofl_switch_config *)data_p;
        of_ipopts_getparams(of_ipopts_info,&result_iv_pairs_p[rec_index]);

      }
      break;
    case CM_ON_DIRECTOR_DATAPATH_SWITCHINFO_APPL_ID:
      {
        struct ofi_switch_info *switch_info = (struct ofi_switch_info *)data_p;
        cntrlrucm_debugmsg("%s(%d):: switch info multipart response",__FUNCTION__,__LINE__);
        of_swinfo_ucm_getparams (switch_info, &result_iv_pairs_p[rec_index]);
      }
      break;
    case CM_ON_DIRECTOR_DATAPATH_METERS_METER_APPL_ID:
      {
        struct ofi_meter_rec_info *meter_entry = (struct ofi_meter_rec_info *)data_p;
        cntrlrucm_debugmsg("%s(%d):: meter config multipart response",__FUNCTION__,__LINE__);
        of_meter_ucm_getparams(meter_entry, &result_iv_pairs_p[rec_index]);
        break;
      }
    case CM_ON_DIRECTOR_DATAPATH_METERS_FEATURES_APPL_ID:
      {
        struct ofi_meter_features_info *meter_features = (struct ofi_meter_features_info *)data_p;
        cntrlrucm_debugmsg("%s(%d):: meter feattures multipart response",__FUNCTION__,__LINE__);
        of_meter_features_ucm_getparams(meter_features, &result_iv_pairs_p[rec_index]);
        break;
      }
    case CM_ON_DIRECTOR_DATAPATH_METERS_STATS_APPL_ID:
      {
        struct ofi_meter_stats_info *meter_stats = (struct ofi_meter_stats_info *)data_p;
        cntrlrucm_debugmsg("%s(%d):: meter stats multipart response",__FUNCTION__,__LINE__);
        of_meter_stats_ucm_getparams(meter_stats, &result_iv_pairs_p[rec_index]);
        break;
      }
    case CM_ON_DIRECTOR_DATAPATH_STATS_BINDSTATS_APPL_ID:
      {
        struct ofi_bind_entry_info *bind_stats = (struct ofi_bind_entry_info*)data_p;
        cntrlrucm_debugmsg("%s(%d):: bind stats multipart response",__FUNCTION__,__LINE__);                      of_bindstats_ucm_getparams(bind_stats, &result_iv_pairs_p[rec_index]);
        break;
      }

    default:
      cntrlrucm_debugmsg("unhandled app...returning failure");
      return;
  }



  total_length=0;
  of_mbuf_put_32(msg_buf,no_of_records);
  total_length+=4;
  msg_buf+=4;

  for(ii=0;ii<no_of_records;ii++)
  {
    length=0;
    UCMDPJECopyIvPairArrayToBuffer(&result_iv_pairs_p[ii],msg_buf,&length);
    msg_buf+=length;
    total_length+=length;
  }

send_reply:
  cntrl_response->length=total_length;
  if (peeraddr_valid_g== TRUE)
  {
    //    cntrlrucm_debugmsg("sending length of message %d",total_length);
    cntrl_ucm_sendresponse(&peer_address_g,(char *)cntrl_response,sizeof(ucmcntrlmsg_t));
  }
  else
  {
    cntrlrucm_debugmsg("no valid peer address to send response");
  }

  if (cntrl_response->multi_part==0)
  {
    //      cntrlrucm_debugmsg("no further multipart response needs to  send ");
    of_memset(&peer_address_g,0,sizeof(struct sockaddr_in));
    peeraddr_valid_g=FALSE;
  }

  if (( cntrl_response->result_code == OF_SUCCESS) && result_iv_pairs_p)
  {
    UCMDPJEFreeIvPairArray (&result_iv_pairs_p[rec_index]);
    of_free(result_iv_pairs_p);
  }
  of_free(cntrl_response);
  return;
}
#endif
