
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
 * File name: dprmtblcbk.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "of_utilities.h"

int32_t of_ipopts_appl_ucmcbk_init (void);
int32_t of_ipopts_modrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs_p,
    struct cm_array_of_iv_pairs * opt_iv_pairs_p,
    struct cm_app_result ** result_p);

int32_t of_ipopts_getexactrec (struct cm_array_of_iv_pairs * pKeysArr,
    struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t of_ipopts_verifycfg (struct cm_array_of_iv_pairs * pKeysArr,
    uint32_t command_id, struct cm_app_result ** result_p);

int32_t of_ipopts_getparams (struct ofl_switch_config *of_ipopts_info,
    struct cm_array_of_iv_pairs * result_iv_pairs_p);


int32_t of_ipopt_ucm_setoptparams (struct cm_array_of_iv_pairs *opt_iv_pairs_p,
    char *reasmCfg, int16_t *miss_snd_len_p, struct cm_app_result ** result_pp);


int8_t reasm_config_b=FALSE;
int8_t miss_snd_len_b=FALSE;
struct cm_dm_app_callbacks of_ipopts_ucm_callbacks = {
  NULL,
  NULL,
  of_ipopts_modrec,
  NULL,
  NULL,
  NULL,
  NULL,
  of_ipopts_getexactrec,
  of_ipopts_verifycfg,
  NULL
};


int32_t of_ipopts_appl_ucmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  cm_register_app_callbacks(CM_ON_DIRECTOR_DATAPATH_IPOPTIONS_APPL_ID, &of_ipopts_ucm_callbacks);
  return OF_SUCCESS;
}


int32_t of_ipopts_modrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs_p,
    struct cm_array_of_iv_pairs * opt_iv_pairs_p,
    struct cm_app_result ** result_p)
{
  uint64_t datapath_handle;
  int32_t  retval= CNTLR_SUCCESS;
  int32_t  status= OF_SUCCESS;
  uint32_t iRetVal=OF_FAILURE;
  uint16_t miss_snd_len;
  char     rsmbconfig[16];
   struct ofp_switch_config     *switch_config_msg;
  struct   cm_app_result *of_ipopts_result = NULL;
  struct   dprm_datapath_general_info datapath_info={};
  struct   of_msg  *msg=NULL;
  void *conn_info_p;

  CM_CBK_DEBUG_PRINT ("Entry");

  if ((of_ipopts_ucm_setmandparams (mand_iv_pairs_p, &datapath_info, &of_ipopts_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    *result_p=of_ipopts_result;
    return OF_FAILURE;
  }

  iRetVal=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
  if (iRetVal != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    *result_p=of_ipopts_result;
    return OF_FAILURE;
  }

 memset(rsmbconfig,0,sizeof(rsmbconfig));
  iRetVal = of_ipopt_ucm_setoptparams(opt_iv_pairs_p, rsmbconfig, &miss_snd_len, &of_ipopts_result);
  if (iRetVal != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    *result_p=of_ipopts_result;    
    return OF_FAILURE;
  }



  CM_CBK_DEBUG_PRINT("rsmbconfig:%s, miss_snd_len:%d\n",rsmbconfig,miss_snd_len);

  if (( reasm_config_b == TRUE ) ||
      ( miss_snd_len_b == TRUE))
  {

    msg = ofu_alloc_message(OFPT_SET_CONFIG,
        OFU_SWITCH_CONFIG_SET_REQ_MESSAGE_LEN);
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," OF message alloc error ");
      return OF_FAILURE;
      //break;
    }
    retval = of_set_switch_config_msg(msg,
        datapath_handle,
        NULL,
        NULL,
         &conn_info_p);
    if(retval != CNTLR_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Set Switch config fail, error=%d ",
          retval);
      return OF_FAILURE;
    }
    /*Configure switch to handle frags normally, no-reassembly*/
    /*TBD configuration support is required to take input*/
    if ( reasm_config_b == TRUE )
    {
      if(!strcmp(rsmbconfig, "OFPC_FRAG_NORMAL"))
      {
        CM_CBK_DEBUG_PRINT("normal");
        ofu_set_switch_to_frag_normal(msg);
      }
      else if(!strcmp(rsmbconfig, "OFPC_FRAG_DROP"))
      {
        CM_CBK_DEBUG_PRINT("drop");
        ofu_set_switch_to_drop_frags(msg);

      }
      else if(!strcmp(rsmbconfig,"OFPC_FRAG_REASM"))
      {
        CM_CBK_DEBUG_PRINT("reasm");
        ofu_set_switch_to_re_assmble_frags(msg);
      }
      else if (!strcmp(rsmbconfig,"OFPC_FRAG_MASK"))
      {
        CM_CBK_DEBUG_PRINT("mask");
        ofu_set_switch_to_mask_frags(msg);
      }
      else
      {
        CM_CBK_DEBUG_PRINT("not valid");
      }
    }
    reasm_config_b=FALSE;
    /*Configure switch to miss send length as default, i.e., 126 bytes*/
    /*TBD configuration support is required to take input*/
    if ( miss_snd_len_b == TRUE)
    {
      CM_CBK_DEBUG_PRINT("miss length is configured %d", miss_snd_len);
      ofu_set_miss_send_len_in_switch(msg, miss_snd_len);
    }
    miss_snd_len_b=FALSE;
   ((struct ofp_switch_config *)msg->desc.start_of_data)->flags = htons(((struct ofp_switch_config *)msg->desc.start_of_data)->flags);
     retval=of_cntrlr_send_request_message(
        msg,
        datapath_handle,
        conn_info_p,
        NULL,
        NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error adding flow entry in route table = %d",retval);
      status = OF_FAILURE;
    }

  }
  return OF_SUCCESS;


}
int32_t of_ipopts_ucm_setmandparams (struct cm_array_of_iv_pairs *mand_iv_pairs_p,
    struct dprm_datapath_general_info *datapath_info, struct cm_app_result ** result_pp)

{
  uint32_t param_count;
  uint64_t idpId;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (param_count = 0; param_count < mand_iv_pairs_p->count_ui;
      param_count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[param_count].id_ui)
    {
      case CM_DM_DATAPATH_DATAPATHID_ID:
        idpId=charTo64bitNum((char *) mand_iv_pairs_p->iv_pairs[param_count].value_p);
        CM_CBK_DEBUG_PRINT("dpid is: %llx", idpId);
        datapath_info->dpid=idpId;
        break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}

int32_t of_ipopt_ucm_setoptparams (struct cm_array_of_iv_pairs *opt_iv_pairs_p,
    char *reasmCfg, int16_t *miss_snd_len_p, struct cm_app_result ** result_pp)
{
  uint32_t param_count;
  uint32_t miss_snd_len;
  CM_CBK_DEBUG_PRINT ("Entered");

  CM_CBK_DEBUG_PRINT("opt_iv_pairs_p->count_ui:%d\n",opt_iv_pairs_p->count_ui);

  for (param_count = 0; param_count < opt_iv_pairs_p->count_ui; param_count++)
  {
    switch (opt_iv_pairs_p->iv_pairs[param_count].id_ui)
    {
      case CM_DM_IPOPTIONS_REASM_ID:
        reasm_config_b = TRUE;
        of_strncpy(reasmCfg, (char *) opt_iv_pairs_p->iv_pairs[param_count].value_p,
            opt_iv_pairs_p->iv_pairs[param_count].value_length);
        CM_CBK_DEBUG_PRINT("reasmCfg:%s\n",reasmCfg);
        break;

      case CM_DM_IPOPTIONS_MISSSENDLENGTH_ID:
        miss_snd_len_b = TRUE;
        miss_snd_len = of_atoi(opt_iv_pairs_p->iv_pairs[param_count].value_p);
        *miss_snd_len_p=(uint16_t)miss_snd_len;
        CM_CBK_DEBUG_PRINT("miss_snd_len:%d  %d\n",*miss_snd_len_p, miss_snd_len );
        break;

      default:
        break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (opt_iv_pairs_p);
  return OF_SUCCESS;

}
int32_t of_ipopts_verifycfg (struct cm_array_of_iv_pairs * mand_iv_pairs_p,
    uint32_t command_id, struct cm_app_result ** result_p)
{
  CM_CBK_DEBUG_PRINT ("Entered");
  return OF_SUCCESS;
}

int32_t of_ipopts_getexactrec (struct cm_array_of_iv_pairs * pKeysArr,
    struct cm_array_of_iv_pairs ** pIvPairArr)
{
  struct   dprm_datapath_general_info datapath_info={};
  uint64_t datapath_handle;
  int32_t iRetVal;  


  CM_CBK_DEBUG_PRINT ("Entered");

  if ((of_ipopts_ucm_setmandparams (pKeysArr, &datapath_info, NULL)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  iRetVal = dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
  if (iRetVal != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("dprm_get_datapath_handle");
    return OF_FAILURE;
  }

  iRetVal=cntl_send_ipopts_cfg_get_request(datapath_handle);
  if ( iRetVal != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("ipoptions config get failed");
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("Ip options will be sent as a multipart response");
  return CM_CNTRL_MULTIPART_RESPONSE;
}


int32_t of_ipopts_getparams (struct ofl_switch_config *of_ipopts_info,
    struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  uint32_t uiIndex = 0,ii;
  char buf[64] = "";



  CM_CBK_DEBUG_PRINT("of_ipopts_info->flags:%x, of_ipopts_info->miss_send_len:%d\n",of_ipopts_info->flags,of_ipopts_info->miss_send_len);
#define CM_IPOPTIONS_CHILD_COUNT 2



  result_iv_pairs_p->iv_pairs = (struct cm_iv_pair *) of_calloc (CM_IPOPTIONS_CHILD_COUNT, sizeof (struct cm_iv_pair));
  if (result_iv_pairs_p->iv_pairs == NULL)
  {
    CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
    return OF_FAILURE;
  }

  switch(of_ipopts_info->flags)
  {
    case  OFPC_FRAG_NORMAL:
      {
        strcpy(buf, "OFPC_FRAG_NORMAL");
      }
      break;
    case OFPC_FRAG_DROP:
      {
        strcpy(buf, "OFPC_FRAG_DROP");
      }
      break;
    case  OFPC_FRAG_REASM:
      {
        strcpy(buf, "OFPC_FRAG_REASM");
      }
      break;
    case OFPC_FRAG_MASK:
      {
        strcpy(buf, "OFPC_FRAG_MASK");
      }
      break;
    default:
      {
        strcpy(buf, "None");

      }
  }
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uiIndex], CM_DM_IPOPTIONS_REASM_ID,
      CM_DATA_TYPE_STRING, buf);
  uiIndex++;


  of_itoa (of_ipopts_info->miss_send_len, buf);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uiIndex], CM_DM_IPOPTIONS_MISSSENDLENGTH_ID, CM_DATA_TYPE_STRING, buf);
  uiIndex++;

  result_iv_pairs_p->count_ui = uiIndex;
  CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
  return OF_SUCCESS;
}


#endif
