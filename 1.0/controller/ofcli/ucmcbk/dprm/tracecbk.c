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
* File name:tracecbk.c
* Author:
* Date: 
* Description: This file is used to set log level using log sevirety as info, warn, debug and error.
* 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "of_utilities.h"
#include "lxoswrp.h"

extern int of_trace_level_g;
int32_t of_tracelog_appl_ucmcbk_init (void);
int32_t of_tracelog_modrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs_p,
    struct cm_array_of_iv_pairs * opt_iv_pairs_p,
    struct cm_app_result ** result_p);

int32_t of_tracelog_getexactrec (struct cm_array_of_iv_pairs * pKeysArr,
    struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t of_tracelog_verifycfg (struct cm_array_of_iv_pairs * pKeysArr,
    uint32_t command_id, struct cm_app_result ** result_p);

struct cm_dm_app_callbacks of_tracelog_ucm_callbacks = {
  NULL,
  NULL,
  of_tracelog_modrec,
  NULL,
  NULL,
  NULL,
  NULL,
  of_tracelog_getexactrec,
  of_tracelog_verifycfg,
  NULL
};


int32_t of_tracelog_appl_ucmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  cm_register_app_callbacks(CM_ON_DIRECTOR_TRACE_APPL_ID, &of_tracelog_ucm_callbacks);
  return OF_SUCCESS;
}


int32_t of_tracelog_modrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs_p,
    struct cm_array_of_iv_pairs * opt_iv_pairs_p,
    struct cm_app_result ** result_p)
{
  uint32_t param_count;
  int16_t loglevel = -1 ;
  uint32_t trace_module;

  for (param_count = 0; param_count < opt_iv_pairs_p->count_ui;
      param_count++)
  {
    switch (opt_iv_pairs_p->iv_pairs[param_count].id_ui)
    {
      case CM_DM_TRACE_MODULE_ID:
        if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "controller")==0)
        {
          trace_module = OF_LOG_MOD;
        }
	else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "cntrlxprt")==0)
        {
          trace_module = OF_LOG_XPRT_PROTO;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "nem")==0)
        {
          trace_module = OF_NEM_LOG;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "route")==0)
        {
          trace_module = OF_RT_LOG;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "peth")==0)
        {
          trace_module = OF_LOG_PETH_APP;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "sampleapp")==0)
        {
          trace_module = OF_LOG_SAMPLE_APP;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "arpnf")==0)
        {
          trace_module = OF_LOG_ARP_NF;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "infraapp")==0)
        {
          trace_module = OF_LOG_INFRA_APP;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "fwdnf")==0)
        {
          trace_module = OF_LOG_FWD_NF;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "ipsec")==0)
        {
          trace_module = OF_LOG_IPSEC;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "netlink")==0)
        {
          trace_module = OF_LOG_NETLINK;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "util")==0)
        {
          trace_module = OF_LOG_UTIL;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "crm")==0)
        {
          trace_module = OF_LOG_CRM;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "nsrm")==0)
        {
          trace_module = OF_LOG_NSRM;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "cntlrtest")==0)
        {
          trace_module = OF_LOG_CNTLR_TEST;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "utill3ext")==0)
        {
          trace_module = OF_LOG_UTIL_L3EXT;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "niciraext")==0)
        {
          trace_module = OF_LOG_EXT_NICIRA;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "ttp")==0)
        {
          trace_module = OF_LOG_TTP;
        }
        #if 1  
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "tsc")==0)
        {
          trace_module = OF_LOG_TSC;
        }
        #endif
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "all")==0)
        {
          trace_module= OF_LOG_MODULE_ALL;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "none")==0)
        {
          trace_module= OF_LOG_MODULE_NONE;
        }
        else
        {
          CM_CBK_DEBUG_PRINT ("Invalid entry! Allowed values are log_mod,nem,rt,peth,mem_pool,ipsec,all ");
          return OF_FAILURE;
        }
        break;

        case CM_DM_TRACE_LEVEL_ID:


        if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "info")==0)
        {
          loglevel = OF_LOG_INFO;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "warn")==0)
        {
          loglevel = OF_LOG_WARN;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "notice")==0)
        {
          loglevel = OF_LOG_NOTICE;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "debug")==0)
        {
          loglevel = OF_LOG_DEBUG;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "error")==0)
        {
          loglevel = OF_LOG_ERROR;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "all")==0)
        {
          loglevel = OF_LOG_ALL;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "critical")==0)
        {
          loglevel = OF_LOG_CRITICAL;
        }
        else if(strcmp(opt_iv_pairs_p->iv_pairs[param_count].value_p, "none")==0)
        {
          loglevel = OF_LOG_NONE;
        }
        else 
        {
          CM_CBK_DEBUG_PRINT ("Invalid entry! Allowed values are all, critical, none, info, warn, error and debug");
          return OF_FAILURE;
        }
        break;
    }
  }
    of_set_trace_level(trace_module,loglevel);
  

  return OF_SUCCESS;
}

int32_t of_tracelog_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *trace_result=NULL;
  uint16_t trace_module_set = 0,param_count;
  uint8_t  is_trace_level_set =0;
  CM_CBK_DEBUG_PRINT ("Entered");
  for (param_count = 0; param_count < key_iv_pairs_p->count_ui;
      param_count++)
  {
    switch (key_iv_pairs_p->iv_pairs[param_count].id_ui)
    {
      case CM_DM_TRACE_MODULE_ID:
          trace_module_set = 1;
        break;
    }
  }
   if(trace_module_set == 0){
    fill_app_result_struct (&trace_result, NULL, CM_GLU_TRACE_MODULE_REQUIRED);
    CM_CBK_DEBUG_PRINT ("Validate Optional Parameters Failed");
    *result_p = trace_result;
    return OF_FAILURE;
  }
  else{
    *result_p = trace_result;
    return OF_SUCCESS;
  }
}

int32_t of_tracelog_getexactrec (struct cm_array_of_iv_pairs * pKeysArr,
    struct cm_array_of_iv_pairs ** pIvPairArr)
{
  int32_t log_level;  
  uint32_t uiIndex = 0,ii;
  char buf[500] = " level  \n";
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
#define CM_TRACE_CHILD_COUNT 2

  CM_CBK_DEBUG_PRINT ("Entered");


  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
    return OF_FAILURE;
  }
  result_iv_pairs_p->iv_pairs =
    (struct cm_iv_pair *) of_calloc (CM_TRACE_CHILD_COUNT, sizeof (struct cm_iv_pair));
  if (result_iv_pairs_p->iv_pairs == NULL)
  {
    CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
    return OF_FAILURE;
  }

  for(ii=0;ii<TOTAL_LOG_MODULES-1; ii++){
    if(trace_params_values[ii].is_trace_module_set == TRACE_MODULE_SET){
      switch(ii){
        case  OF_LOG_MOD:
          {
            strcat(buf, "\tOF_LOG_CONTROLLER : ");
          }
          break;
        case OF_NEM_LOG:
          {
            strcat(buf, "\tOF_NEM_LOG : ");
          }
          break;
        case  OF_RT_LOG:
          {
            strcat(buf, "\tOF_ROUTE_LOG :  ");
          }
          break;
        case OF_LOG_PETH_APP:
          {
            strcat(buf, "\tOF_LOG_PETH_APP: ");
          }
          break;
        case OF_LOG_SAMPLE_APP:
          {
            strcat(buf, "\tOF_LOG_SAMPLE_APP : ");
          }
          break;
        case OF_LOG_ARP_NF:
          {
            strcat(buf, "\tOF_LOG_ARP_NF : ");
          }
          break;
        case OF_LOG_INFRA_APP:
          {
            strcat(buf, "\tOF_LOG_INFRA_APP : ");
          }
          break;
        case OF_LOG_FWD_NF:
          {
            strcat(buf, "\tOF_LOG_FWD_NF : ");
          }
          break;
        case OF_LOG_IPSEC:
          {
            strcat(buf, "\tOF_LOG_IPSEC : ");
          }
          break;
        case OF_LOG_NETLINK:
          {
            strcat(buf, "\tOF_LOG_NETLINK : ");
          }
          break;
        case OF_LOG_UTIL:
          {
            strcat(buf, "\tOF_LOG_UTIL : ");
          }
          break;
        case OF_LOG_CRM:
          {
            strcat(buf, "\tOF_LOG_CRM : ");
          }
          break;
         case OF_LOG_NSRM:
          {
            strcat(buf, "\tOF_LOG_NSRM : ");
          }
          break;
         case OF_LOG_CNTLR_TEST:
          {
            strcat(buf, "\tOF_LOG_CNTLR_TEST : ");
          }
          break;
        case OF_LOG_UTIL_L3EXT:
          {
            strcat(buf, "\tOF_LOG_UTIL_L3EXT : ");
          }
          break;
        case OF_LOG_EXT_NICIRA:
          {
            strcat(buf, "\tOF_LOG_EXT_NICIRA : ");
          }
          break;
        case OF_LOG_TTP:
          {
            strcat(buf, "\tOF_LOG_TTP : ");
          }
          break;
        case OF_LOG_TSC:
          {
            strcat(buf, "\tOF_LOG_TSC : ");
          }
          break;

        /* 
        case OF_LOG_ALL:
          {
            strcat(buf, "\tOF_LOG_ALL : ");
          }
          break;
        case OF_LOG_NONE:
          {
            strcat(buf, "\tOF_LOG_NONE : ");
          }
          break;
        */

      }
      switch(trace_params_values[ii].trace_level){
        case  OF_LOG_INFO:
          {
            strcat(buf, "info \n");
          }
          break;
        case OF_LOG_DEBUG:
          {
            strcat(buf, "debug \n");
          }
          break;
        case OF_LOG_NOTICE:
          {
            strcat(buf, "notice \n");
          }
          break;
        case  OF_LOG_ERROR:
          {
            strcat(buf, "error \n ");
          }
          break;
        case OF_LOG_WARN:
          {
            strcat(buf, "warn \n");
          }
          break;
        case OF_LOG_ALL:
          {
            strcat(buf, "all \n");
          }
          break;
        case OF_LOG_CRITICAL:
          {
            strcat(buf, "critical \n");
          }
          break;
        case OF_LOG_NONE:
          {
            strcat(buf, "none \n ");
          }
          break;
        default:
          {
            strcat(buf, "invalid ");
          }
          break;
      }
    }

  }


  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uiIndex], CM_DM_TRACE_MODULE_ID,
      CM_DATA_TYPE_STRING, buf);
  uiIndex++;

  result_iv_pairs_p->count_ui = uiIndex;
  *pIvPairArr = result_iv_pairs_p;
  CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
  return OF_SUCCESS;
}

#endif
