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
 * File name: jehandlr.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description: 
*/

#ifdef CM_SUPPORT
#ifdef CM_JE_SUPPORT

#include "jeincld.h"
#include "jeldef.h"
#include "jever.h"
#include "jeldef.h"
#include "jelif.h"
#include "errno.h"

#include "dmerr.h"
#include "evntgdef.h"
#include "evntgif.h"

//#include "ucmmesg.h"
#include "jesyslog.h"
/******************************************************************************
 * * * * * * * * * * * * * * * GLOBAL VARIABLES* * * * * * * * * * * * * * * * 
 *****************************************************************************/
extern JESMFuncPtr cm_je_state_machine_table[UCMJE_MAX_STATE][UCMJE_MAX_EVENT];
extern struct cm_dm_channel_callbacks *cm_channels_p[CM_MAX_CHANNEL_ID];
#ifdef CM_JE_STATS_SUPPORT
extern int32_t cm_je_global_stats_g[JE_STATS_MAX];
extern int32_t cm_je_session_stats_g[JE_SESSION_STATS_MAX];
extern int32_t cm_je_appl_stats_g[JE_SECAPPL_STATS_MAX];
#endif
extern unsigned char bLdsvInitialized_g;
extern uiInt32Size_g;
extern uiInt64Size_g;
extern uiOSIntSize_g;
#ifdef CM_ROLES_PERM_SUPPORT
extern cm_auth_user pRegLoginAuthFunPtr;
#endif
extern uint64_t run_time_version_g;
extern uint64_t save_version_g;

extern unsigned char bLoadInProgress_g;
extern unsigned char bSaveInProgress_g;

/******************************************************************************
 * * * * * * * * * * * * * FUNCTION DEFINITIONS * * * * * * * * * * * * * * * * 
 *****************************************************************************/
/**
  \ingroup JEAPI
  This API reads Input Data for a Configuratin Session and calls state machine API 
  based on Input command.
  If input is Start Command it creates a configuration session and returns its pointer
  to management engines.
  it calls Update Handler to add a new command, Commit Handler for Commit command,
  and revoke handler to revoke command.

  <b>Input paramameters: </b>\n
  <b><i> config_session_p: </i></b> Pointer to Configuration Session for which request
  belongs to 
  <b><i> config_request_p: </i></b> pointer to Configuration Request sent by Management
  Engines.

  <b>Output Parameters: </b>
  <b><i>pUCMResult:</i></b> double pointer to UCMResult 
  In Success case it contains data for Get Requests
  In Failure case error information \n

  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */

/******************************************************************************
 ** Function Name : cm_je_config_session_handler 
 ** Description   : This API reads Input Data and calls state machine handler
 ** Input params  : config_session_p - Pointer to Configuration Session
 **                 config_request_p - pointer to Configuration Request sent by Management
 **                              Engines.
 ** Output params : result_p - double pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/

int32_t cm_je_config_session_handler (void * pInCfgSession,
      struct je_config_request * config_request_p,
      struct cm_result ** presult_p)
{
   struct je_config_session *config_session_p = NULL;
   struct cm_result *pSessionResult = NULL;
   char *dm_path_p;
   int32_t return_value;
   uint32_t event;

   if (config_request_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid input");
      return OF_FAILURE;
   }
   CM_INC_STATS_JE(JE_CFGSESSIONREQ_RECIEVES);

   dm_path_p=config_request_p->dm_path_p;
   pSessionResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (pSessionResult == NULL)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation failed for result structure");
      return OF_FAILURE;
   }
#if 0/*This is not need here...update handler will check for roles*/
#ifdef CM_ROLES_PERM_SUPPORT
   if (return_value =
         cm_verify_role_permissions (dm_path_p, config_request_p->admin_role,
            config_request_p->sub_command_id) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Permission Denied");
      pSessionResult->result_code = CM_JE_FAILURE;
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pSessionResult,
            return_value, dm_path_p);
      *presult_p = pSessionResult;
      return OF_FAILURE;
   }
#endif
#endif
   switch (config_request_p->command_id)
   {
      case CM_COMMAND_CONFIG_SESSION_START:
         CM_JE_DEBUG_PRINT ("Session Start Command");
         if (pInCfgSession)
         {
            CM_JE_DEBUG_PRINT ("Session already exists");
            pSessionResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT(pSessionResult,
                  UCMJE_ERROR_CONFSESSION_ALREADY_EXISTS,
                  dm_path_p);
            *presult_p = pSessionResult;
            return OF_FAILURE;
         }
         config_session_p =
            (struct je_config_session *) cmje_config_session_start (config_request_p);
         if (config_session_p)
         {
            /* return Config Session to Management Engine */
            CM_JE_DEBUG_PRINT ("Copying Success Message");
            pSessionResult->result_code = CM_JE_SUCCESS;
            /*Assigning  Configuration Session pointer to result */
            pSessionResult->data.je_data_p = (struct je_config_session *) config_session_p;
            *presult_p = pSessionResult;
            return OF_SUCCESS;
         }
         else
         {
            CM_JE_DEBUG_PRINT ("Copying Failure Message");
            (pSessionResult)->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT(pSessionResult,
                  UCMJE_ERROR_CONFSESSION_START_FAILED,
                  dm_path_p);
            *presult_p = pSessionResult;
            return OF_FAILURE;
         }
         break;
      case CM_COMMAND_CONFIG_SESSION_UPDATE:
         CM_JE_DEBUG_PRINT ("Session Update Command");
         if (!pInCfgSession)
         {
            CM_JE_DEBUG_PRINT ("Session is NULL");
            pSessionResult->result_code = CM_JE_FAILURE;
            pSessionResult->result.error.error_code = UCMJE_ERROR_CONFIGSESSION_NULL;
            *presult_p = pSessionResult;
            return OF_FAILURE;
         }
         config_session_p = (struct je_config_session *) pInCfgSession;
         config_session_p->state = UCMJE_CFGSESS_ACTIVE_STATE;
         event = UCMJE_UPDATE_EVENT;
         break;
      case CM_COMMAND_CONFIG_SESSION_END:
         CM_JE_DEBUG_PRINT ("Session End Command");
         if (!pInCfgSession)
         {
            CM_JE_DEBUG_PRINT ("Session is NULL");
            pSessionResult->result_code = CM_JE_FAILURE;
            pSessionResult->result.error.error_code = UCMJE_ERROR_CONFIGSESSION_NULL;
            *presult_p = pSessionResult;
            return OF_FAILURE;
         }
         config_session_p = (struct je_config_session *) pInCfgSession;
         config_session_p->state = UCMJE_CFGSESS_ACTIVE_STATE;
         if (config_request_p->sub_command_id == CM_CMD_CONFIG_SESSION_COMMIT)
         {
            event = UCMJE_COMMIT_EVENT;
         }

         if (config_request_p->sub_command_id == CM_CMD_CONFIG_SESSION_REVOKE)
         {
            event = UCMJE_REVOKE_EVENT;
         }
         break;
      case CM_COMMAND_CONFIG_SESSION_TIMEOUT:
         CM_JE_DEBUG_PRINT ("Session TimeOut Command");
         if (!pInCfgSession)
         {
            CM_JE_DEBUG_PRINT ("Session is NULL");
            pSessionResult->result_code = CM_JE_FAILURE;
            pSessionResult->result.error.error_code = UCMJE_ERROR_CONFIGSESSION_NULL;
            *presult_p = pSessionResult;
            return OF_FAILURE;
         }
         config_session_p = (struct je_config_session *) pInCfgSession;
         config_session_p->state = UCMJE_CFGSESS_ACTIVE_STATE;
         event = UCMJE_TIMEOUT_EVENT;
         break;

      case CM_COMMAND_CONFIG_SESSION_GET:
         CM_JE_DEBUG_PRINT ("Configuration Session Get Command");
         if (!pInCfgSession)
         {
            CM_JE_DEBUG_PRINT ("Session is NULL");
            pSessionResult->result_code = CM_JE_FAILURE;
            pSessionResult->result.error.error_code = UCMJE_ERROR_CONFIGSESSION_NULL;
            *presult_p = pSessionResult;
            return OF_FAILURE;
         }
         config_session_p = (struct je_config_session *) pInCfgSession;
         config_session_p->state = UCMJE_CFGSESS_ACTIVE_STATE;
         event = UCMJE_GET_EVENT;
         break;

      default:
         CM_JE_DEBUG_PRINT ("unknown  Command");
         pSessionResult->result_code = CM_JE_FAILURE;
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pSessionResult,
               UCMJE_ERROR_UNKNOWN_SUB_COMMAND,
               dm_path_p);
         *presult_p = pSessionResult;
         return OF_FAILURE;
   }

   if(event < UCMJE_MAX_EVENT) //CID 550
   {
      return_value =
         cm_je_state_machine_table[config_session_p->state][event] (config_session_p,
               config_request_p,
               pSessionResult);
   }
   else
      return_value = OF_FAILURE;

   if (return_value == OF_SUCCESS)
   {
      pSessionResult->result_code = CM_JE_SUCCESS;
   }
   else
   {
      pSessionResult->result_code = CM_JE_FAILURE;
   }
   *presult_p = pSessionResult;
   return OF_SUCCESS;
}

/**
  \ingroup JEAPI
  This API reads Input Data for a Get Request, identifies for which Data Model Node
  request has come and invokes its registered callback function. Copies Security
  Application Response into UCMResult structure.

  <b>Input paramameters: </b>\n
  <b><i> config_session_p: </i></b> Pointer to Configuration Session for which request
  belongs to 
  <b><i> config_request_p: </i></b> pointer to Configuration Request sent by Management
  Engines.

  <b>Output Parameters: </b>
  <b><i>pUCMResult:</i></b> double pointer to UCMResult 
  In Success case it contains data for Get Requests
  In Failure case error information \n

  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */

/******************************************************************************
 ** Function Name : cm_je_appl_show_request_handler 
 ** Description   : This API reads Input Data for a Get Request, identifies for which
 **                  Data Model Node request has come and invokes its registered callback
 **                 function.
 ** Input params  : config_session_p - Pointer to Configuration Session
 **                 config_request_p - pointer to Configuration Request sent by Management
 **                              Engines.
 ** Output params : result_p - double pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/

int32_t cm_je_appl_show_request_handler (struct je_config_request * config_request_p,
      struct cm_result ** presult_p)
{
   struct cm_result *sec_appl_result_p = NULL;
   struct cm_array_of_iv_pairs KeysIvArray = { };
   struct cm_array_of_iv_pairs *result_iv_array_p = NULL;
   struct cm_array_of_iv_pairs PrevRecKey={};
   uint32_t uiRecCount = 0;
   uint32_t uiCnt = 0,channel_id_ui = 0,appl_id_ui = 0;
   uint32_t error_code = OF_SUCCESS;
   struct cm_dm_data_element *pDMNode = NULL;
   cm_channel_getfirst_nrecs_cbk_p pChannelGetFirstNRecFn;
   cm_channel_getnext_nrecs_cbk_p pChannelGetNextNRecFn;
   cm_channel_getexact_rec_cbk_p pChannelGetExactRecFn;
   struct cm_array_of_nv_pair *key_nv_pairs_p=NULL;
   struct cm_array_of_nv_pair *dmpath_key_nv_pairs_p=NULL;
   char *dm_path_p = NULL;
   char *parent_path_p = NULL;
   char aLogMesgDMPath[128] = "igd.logmesg";
   int32_t icRet;
   uint32_t mgmt_engine_id=0;

   if (config_request_p->command_id != CM_COMMAND_APPL_REQUEST)
   {
      CM_JE_DEBUG_PRINT ("Not a Security Application Request");
      return OF_FAILURE;
   }

   mgmt_engine_id = config_request_p->mgmt_engine_id;

   CM_INC_STATS_JE(JE_SECAPPLREQ_RECIEVES);
   CM_INC_STATS_JESECAPPL(JE_SECAPPL_GET_REQ_COUNT);

   sec_appl_result_p = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (sec_appl_result_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation failed for result structure");
      return OF_FAILURE;
   }

   do
   {
      dm_path_p = config_request_p->dm_path_p;
      if (!dm_path_p)
      {
         CM_JE_DEBUG_PRINT ("DMPath is NULL");
         error_code=UCMJE_ERROR_DMPATH_NULL;
         break;
      }

      pDMNode = (struct cm_dm_data_element *)cmi_dm_get_dm_node_from_instance_path
         (config_request_p->dm_path_p,of_strlen (config_request_p->dm_path_p));

      /* Application Callbacks */
      if (!(pDMNode))
      {
         CM_JE_DEBUG_PRINT ("DM Node is NULL");
         error_code=UCMJE_ERROR_GETNODE_BYDMPATH;
         break;
      }
#ifdef CM_ROLES_PERM_SUPPORT

      if(pDMNode->element_attrib.non_config_leaf_node_b != FALSE)
      {
         parent_path_p = of_calloc(1, of_strlen(dm_path_p)+1);
         cm_remove_last_node_instance_from_dmpath(dm_path_p,parent_path_p);
         dm_path_p = parent_path_p;
      }

      if (error_code =
            cm_verify_role_permissions (dm_path_p, config_request_p->admin_role,
               config_request_p->sub_command_id) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Permission Denied");
#ifdef CM_AUDIT_SUPPORT
         UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_PERMISSION_DENIED,config_request_p->admin_name,config_request_p->admin_role,
               config_request_p->sub_command_id, dm_path_p, NULL,NULL);
#endif
         error_code=UCMJE_ERROR_PERMISSION_DENIED;
         if(parent_path_p)
         {
            of_free(parent_path_p);
            parent_path_p = NULL;
         }
         break;
      }

      if(parent_path_p)
      {
         CM_JE_DEBUG_PRINT ("freeing parent path");
         of_free(parent_path_p);
         parent_path_p = NULL;
      }
      dm_path_p = config_request_p->dm_path_p;
#endif

      if ( cmi_dm_get_key_array_from_namepath (dm_path_p,
               &dmpath_key_nv_pairs_p)
            != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Get Keys Array From Name Path failed");
         error_code= UCMJE_ERROR_DMGET_KEYS_FROM_NAMEPATH_FAILED;
         break;
      }

      if (je_combine_nvpairs (NULL,  dmpath_key_nv_pairs_p,
               &(config_request_p->data.sec_appl.key_nv_pairs),
               &key_nv_pairs_p) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Combining Nv Pair Arrays failed");
         error_code= UCMJE_ERROR_DMGET_KEYS_FROM_NAMEPATH_FAILED;
         break;
      }


      /* Copy Input Keys Array */
      if (key_nv_pairs_p || mgmt_engine_id)
      {
         if (error_code = je_copy_nvpairarr_to_ivpairarr (dm_path_p,
                  key_nv_pairs_p, &KeysIvArray,mgmt_engine_id)
               != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Copy NvPair Arr to IVPairArr Failed");
            break;
         }
      }

      uiRecCount = config_request_p->data.sec_appl.count_ui;
      if (uiRecCount == 0 && (config_request_p->sub_command_id != CM_CMD_GET_EXACT_REC))
      {
         CM_JE_DEBUG_PRINT ("Invalid Record Count is zeor");
         error_code= UCMJE_ERROR_GET_REQUEST_RECCOUNT_ZERO;
         break;
      }

      if((config_request_p->sub_command_id == CM_CMD_GET_FIRST_N_LOGS)|| 
            (config_request_p->sub_command_id == CM_CMD_GET_NEXT_N_LOGS) )
      {
         icRet=cm_get_channel_id_and_appl_id(aLogMesgDMPath,&appl_id_ui,&channel_id_ui);
      }
      else
      {
         icRet=cm_get_channel_id_and_appl_id(dm_path_p,&appl_id_ui,&channel_id_ui);
      }

      if(icRet!=OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Get Application and Channel Id failed");
         error_code= UCMJE_ERROR_UNKNOWN_TRCHANNEL;
         break;
      }  

      switch (config_request_p->sub_command_id)
      {
         case CM_CMD_GET_FIRST_N_LOGS:
         case CM_CMD_GET_FIRST_N_RECS:
            CM_JE_DEBUG_PRINT ("Get First Records");
            CM_INC_STATS_JESECAPPL(JE_SECAPPL_GET_FIRST_N_RECS);
            //pGetFirstNRecFn = pDMNode->app_cbks_p->cm_getfirst_nrecs;
            pChannelGetFirstNRecFn=cm_channels_p[channel_id_ui]->cm_getfirst_nrecs;
            if (!pChannelGetFirstNRecFn)
            {
               CM_JE_DEBUG_PRINT ("GETFirstNRecords Callback is NULL");
               error_code= UCMJE_ERROR_GETFIRSTCBK_NULL;
               break;                                                 
            }
            error_code = pChannelGetFirstNRecFn (appl_id_ui,&KeysIvArray, &uiRecCount, &result_iv_array_p);
            if ((error_code != OF_SUCCESS) || (uiRecCount == 0))
            {
               CM_JE_DEBUG_PRINT ("GETFirstNRecords Callback returned Failure");
               error_code=UCMJE_ERROR_GETFIRSTCBK_FAILED;
            }
            break;

         case CM_CMD_GET_NEXT_N_LOGS:
         case CM_CMD_GET_NEXT_N_RECS:
            CM_JE_DEBUG_PRINT ("Get Next Records");
            CM_INC_STATS_JESECAPPL(JE_SECAPPL_GET_NEXT_N_RECS);

            /* Copy Input Keys Array */
            if ((error_code = je_copy_nvpairarr_to_ivpairarr (dm_path_p,
                        &(config_request_p->data.sec_appl.prev_rec_key_nv_pairs),
                        &PrevRecKey, mgmt_engine_id))
                  != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Copy NvPair Arr to IVPairArr Failed");
               break;
            }
            //pGetNextNRecFn = pDMNode->app_cbks_p->cm_getnext_nrecs;
            pChannelGetNextNRecFn=cm_channels_p[channel_id_ui]->cm_getnext_nrecs;
            if (!pChannelGetNextNRecFn)
            {
               CM_JE_DEBUG_PRINT ("GETNextNRecords Callback is NULL");
               error_code= UCMJE_ERROR_GETNEXTCBK_NULL;
               break;
            }
            error_code =
               pChannelGetNextNRecFn (appl_id_ui,&KeysIvArray, &PrevRecKey, &uiRecCount,
                     &result_iv_array_p);

            if ((error_code != OF_SUCCESS) || (uiRecCount == 0))
            {
               CM_JE_DEBUG_PRINT ("GETNextNRecords Callback Failred");
               error_code=UCMJE_ERROR_GETNEXTCBK_FAILED;
            }
            break;
         case CM_CMD_GET_EXACT_REC:
            CM_JE_DEBUG_PRINT ("Get Exact Records");
            CM_INC_STATS_JESECAPPL(JE_SECAPPL_GET_EXACT_REC);
            if (pDMNode->element_attrib.scalar_group_b != TRUE)
            {
               error_code = je_validate_command (config_request_p->sub_command_id, config_request_p->dm_path_p,
                     config_request_p->data.sec_appl.key_nv_pairs,sec_appl_result_p);
               if ( error_code != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("GETExactRecord error_code =%ld",error_code);
                  sec_appl_result_p->result_code = CM_JE_FAILURE;
                  break;
               }
            }
            //pGetExactRecFn = pDMNode->app_cbks_p->cm_getexact_rec;
            pChannelGetExactRecFn=cm_channels_p[channel_id_ui]->cm_getexact_rec;
            if (!pChannelGetExactRecFn)
            {
               CM_JE_DEBUG_PRINT ("GETExactRecord Callback is NULL");
               error_code= UCMJE_ERROR_GETEXACT_NULL;
               break;
            }
            error_code = pChannelGetExactRecFn(appl_id_ui,&KeysIvArray, &result_iv_array_p);
            if (error_code != OF_SUCCESS)
            {
               error_code=UCMJE_ERROR_GETEXACT_FAILED;
               break;
            }
            uiRecCount = 1;
            break;
         default:
            CM_JE_DEBUG_PRINT ("unknown  Command");
            error_code= UCMJE_ERROR_INVALID_APLGET_CMD;
            break;
      }
      if( error_code != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT("Failure case : Coming out of while loop");
         break;
      }

      CM_JE_DEBUG_PRINT ("Success from Security Application");

      /* Copy Record count into result */
      sec_appl_result_p->data.sec_appl.count_ui = uiRecCount;

      sec_appl_result_p->data.sec_appl.nv_pair_array = (struct cm_array_of_nv_pair *) of_calloc
         (sec_appl_result_p->data.sec_appl.count_ui, sizeof (struct cm_array_of_nv_pair));
      if (!(sec_appl_result_p->data.sec_appl.nv_pair_array))
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation failed");
         error_code=  UCMJE_ERROR_MEMORY_ALLOCATION_FAILED;
         break;
      }

      /* Convert Result IvPairs into nv_pairs */
      if (error_code = je_copy_arr_ivpairarr_to_arr_nvpairarr (dm_path_p,
               result_iv_array_p,
               sec_appl_result_p->data.sec_appl.count_ui,
               sec_appl_result_p->data.sec_appl.nv_pair_array) !=
            OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Copy NvPair Arr to IVPairArr Failed");
         sec_appl_result_p->result_code = CM_JE_FAILURE;
         break;
      }

   } while (FALSE);

	 cm_je_free_ivpair_array (&PrevRecKey);
	 cm_je_free_ivpair_array (&KeysIvArray);
	 if(dmpath_key_nv_pairs_p)
	 {
			cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
			dmpath_key_nv_pairs_p = NULL;
	 }
	 if(key_nv_pairs_p)
	 {
			cm_je_free_ptr_nvpairArray (key_nv_pairs_p);
			key_nv_pairs_p = NULL;
	 }

   /* Free Result IvArray */
   if (result_iv_array_p)
	 {
      CM_JE_DEBUG_PRINT ("uiRecCount=%d result_iv_array_p[%d].count_ui %d",uiRecCount,uiCnt, result_iv_array_p[uiCnt].count_ui);
			for (uiCnt = 0; uiCnt < uiRecCount; uiCnt++)
			{
				 cm_je_free_ivpair_array (&result_iv_array_p[uiCnt]);
			}
			of_free (result_iv_array_p);
			result_iv_array_p = NULL;
	 }
   *presult_p = sec_appl_result_p;

   if( error_code != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Failure from Security Application");
      CM_INC_STATS_JESECAPPL(JE_SECAPPL_GET_REQ_FAILED);
      sec_appl_result_p->result_code = CM_JE_FAILURE;
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (sec_appl_result_p,
            error_code, dm_path_p);
      return OF_FAILURE;
   }

   sec_appl_result_p->result_code = CM_JE_SUCCESS;
   return OF_SUCCESS;
}

#ifdef CM_STATS_COLLECTOR_SUPPORT

int32_t cm_je_fill_error(char *dm_path_p, struct cm_result *sec_appl_result_p , int32_t error_code)
{
   sec_appl_result_p->result_code = CM_JE_FAILURE;
   UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (sec_appl_result_p,
         error_code, dm_path_p);
   return OF_SUCCESS;
}

int32_t cm_je_send_request_to_stats (struct je_config_request * config_request_p,
      struct cmje_tnsprtchannel_info *pCurrentChannel,
      struct cm_result ** presult_p)
{
   struct cm_tnsprt_channel *pStatsChannel=NULL;
   struct cm_command CommandInfo;
   struct cm_role_info role_info;
   struct cmje_tnsprtchannel_info *pStatsTrChannel = NULL;
   struct cm_tnsprt_channel CurrChannel;

   of_memset(&CurrChannel,0,sizeof(struct cm_tnsprt_channel)); 
   of_memset(&CommandInfo,0,sizeof(struct cm_command));
   of_memset(&role_info,0,sizeof(struct cm_role_info));

   je_fill_command_info(config_request_p->sub_command_id, config_request_p->dm_path_p,
         0, NULL, &CommandInfo);

   pStatsChannel = (struct cm_tnsprt_channel *) cm_tnsprt_create_channel (
         CM_IPPROTO_TCP,
         CM_LOOPBACK_ADDR, 0,
         UCMSTATS_COLLECTOR_PORT);
   if (!pStatsChannel)
   {
      CM_JE_DEBUG_PRINT ("%s :: cm_tnsprt_create_channel failed\n\r", __FUNCTION__);
      return OF_FAILURE;
   }

   pStatsTrChannel = (struct cmje_tnsprtchannel_info *) cm_je_tnsprtchannel_alloc ();
   if (!pStatsTrChannel)
   {
      CM_JE_DEBUG_PRINT ("Transport Channel Memory Allocation Failed");
      cm_tnsprt_close_channel(pStatsChannel);
      return OF_FAILURE;
   }
   pStatsTrChannel->sock_fd_ui = pStatsChannel->sock_fd_i;
   cm_timer_create (&(pStatsTrChannel->session_timer_p));

   /* Start Idle Timer for Transport Channel */
   cm_timer_start (pStatsTrChannel->session_timer_p,
         (void *) cmje_tnsprtchannel_timeout, (void *)pStatsTrChannel,
         SECS_TMR, UCMJE_INAC_TIMEOUT, 0);

   of_strcpy(role_info.admin_name, config_request_p->admin_name);
   of_strcpy(role_info.admin_role, config_request_p->admin_role);

   CurrChannel.sock_fd_i = pCurrentChannel->sock_fd_ui;
   if(UcmStatsSndRqToCoalescar(pStatsChannel, &CommandInfo, &role_info, &CurrChannel)!= OF_SUCCESS)
   {
      cm_tnsprt_close_channel(pStatsChannel);
      cm_je_tnsprtchannel_free (pStatsChannel);
      return OF_FAILURE;
   }

   if(cmje_add_tnsprtchannel_to_list(pStatsTrChannel) != OF_SUCCESS)
   {
      printf("could not add stats sockfd to listen list");
      cm_tnsprt_close_channel(pStatsChannel);
      cm_je_tnsprtchannel_free (pStatsChannel);
      return OF_FAILURE;
   }

   pCurrentChannel->child_sock_fd_ui = pStatsTrChannel->sock_fd_ui;
   return OF_SUCCESS ;
}

int32_t cm_je_stats_request_handler (struct cmje_tnsprtchannel_info *tnsprt_channel_p,
      struct je_config_request * config_request_p,
      struct cm_result ** presult_p)
{
   char *dm_path_p=NULL;
   struct cm_dm_data_element *pDMNode=NULL;
   struct cm_result  *sec_appl_result_p = NULL;
   int32_t error_code;
   //  uint32_t  uiNvCnt;
   struct cm_tnsprt_channel *pCurrentChannel=NULL;

   CM_JE_DEBUG_PRINT ("\nReceived Stats Request\n");

   sec_appl_result_p = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (sec_appl_result_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation failed for result structure");
      return OF_FAILURE;
   }


   dm_path_p = config_request_p->dm_path_p;
   if (!dm_path_p)
   {
      CM_JE_DEBUG_PRINT ("DMPath is NULL");
      error_code=UCMJE_ERROR_DMPATH_NULL;
      cm_je_fill_error(dm_path_p, sec_appl_result_p, error_code);
      *presult_p = sec_appl_result_p;
      return OF_FAILURE;
   }

   if(config_request_p->command_id == CM_COMMAND_STATS_REQUEST)
   { 
      pDMNode = (struct cm_dm_data_element *)cmi_dm_get_dm_node_from_instance_path
         (dm_path_p,of_strlen (config_request_p->dm_path_p));

      if (!(pDMNode))
      {
         CM_JE_DEBUG_PRINT ("DM Node is NULL");
         error_code=UCMJE_ERROR_GETNODE_BYDMPATH;
         cm_je_fill_error(dm_path_p, sec_appl_result_p, error_code);
         *presult_p = sec_appl_result_p;
         return OF_FAILURE;
      } 

      if(pDMNode->element_attrib.stats_command_b != TRUE)
      {
         CM_JE_DEBUG_PRINT("Not a stats command");        
         error_code=UCMJE_ERROR_GETNODE_BYDMPATH;
         cm_je_fill_error(dm_path_p, sec_appl_result_p, error_code);
         *presult_p = sec_appl_result_p;
         return OF_FAILURE;
      }

      pCurrentChannel= of_calloc(1,sizeof(struct cm_tnsprt_channel));
      pCurrentChannel->sock_fd_i = tnsprt_channel_p->sock_fd_ui;

      if(cm_je_send_request_to_stats(config_request_p,tnsprt_channel_p, presult_p)!=OF_SUCCESS)
      {
         sec_appl_result_p->result_code = CM_JE_FAILURE; 
         /*need to add enum */
         error_code=UCMJE_ERROR_DMPATH_NULL;
         cm_je_fill_error(dm_path_p, sec_appl_result_p, error_code);
         *presult_p = sec_appl_result_p;
         return OF_FAILURE;
      }

      of_free(sec_appl_result_p);
      sec_appl_result_p = NULL;

      CM_JE_DEBUG_PRINT ("\nSent Stats Request to statscollector module\n");
   }
   else if(config_request_p->command_id == CM_COMMAND_STATS_RESPONSE)
   {
      if(config_request_p->data.stats_data.result_code == OF_SUCCESS)
      {
         sec_appl_result_p->result_code = OF_SUCCESS;
         sec_appl_result_p->data.stats_data.nv_pair_flat_buf_len_ui =
            config_request_p->data.stats_data.nv_pair_flat_buf_len_ui;
         sec_appl_result_p->data.stats_data.nv_pair_flat_buf_p=
            config_request_p->data.stats_data.nv_pair_flat_buf_p;
      }
      else
      {    
         sec_appl_result_p->result_code = CM_JE_FAILURE;
         error_code=UCMJE_ERROR_DMPATH_NULL;
         cm_je_fill_error(dm_path_p, sec_appl_result_p, error_code);
      }
   }
   else
   {
      sec_appl_result_p->result_code = OF_FAILURE;
      *presult_p = sec_appl_result_p;
      return OF_FAILURE;
   }

   *presult_p = sec_appl_result_p;
   return OF_SUCCESS;
}

#endif 

/**
  \ingroup JEAPI
  This API reads Input Data for a Data Model Get Request,
  invokes Data Model API and frames the result into Result structure

  <b>Input paramameters: </b>\n
  <b><i> config_session_p: </i></b> Pointer to Configuration Session for which request
  belongs to 
  <b><i> config_request_p: </i></b> pointer to Configuration Request sent by Management
  Engines.

  <b>Output Parameters: </b>
  <b><i>pUCMResult:</i></b> double pointer to UCMResult 
  In Success case it contains data for Get Requests
  In Failure case error information \n

  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */

/******************************************************************************
 ** Function Name : cm_je_dm_get_request_handler 
 ** Description   : This API reads Input Data for a Data Model Get Request,
 **                  invokes Data Model API and frames the result into Result structure
 ** Input params  : config_session_p - Pointer to Configuration Session
 **                 config_request_p - pointer to Configuration Request sent by Management
 **                              Engines.
 ** Output params : result_p - double pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cm_je_dm_get_request_handler (struct je_config_request * config_request_p,
      struct cm_result ** presult_p)
{
   struct cm_result *pDMResult = NULL;
   uint32_t return_value = 0, count_ui = 0;
   struct cm_dm_data_element *pDMNode = NULL;
   struct cm_array_of_structs *node_info_array_p = NULL;
   struct cm_array_of_nv_pair *keys_array_p = NULL;
   struct cm_dm_node_info *node_info_p = NULL;
   char *dm_path_p = NULL;
   char *pDMNamePath = NULL;
   char *parent_path_p = NULL;
   void *opaque_info_p = NULL;
   uint32_t opaque_info_length = 0;
   struct cm_dm_instance_map *pMap = NULL;
   char *file_name_p;
   char *directory_name_p;
   unsigned char bInstExists=FALSE;
   unsigned char parent_trans_b=FALSE;
#ifdef CM_ROLES_PERM_SUPPORT
   struct cm_dm_array_of_role_permissions *role_perm_ary_p;
#endif
   if (config_request_p->command_id != CM_COMMAND_DM_REQUEST)
   {
      CM_JE_DEBUG_PRINT ("Not a Data Model Request");
      return OF_FAILURE;
   }

   CM_INC_STATS_JE(JE_DMREQ_RECIEVES);
   pDMResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (pDMResult == NULL)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation failed for result structure");
      return OF_FAILURE;
   }

   dm_path_p = config_request_p->dm_path_p;

   if(dm_path_p)
   {
      pDMNamePath = of_calloc(1, of_strlen(dm_path_p)+1);

      if ((cmi_dm_remove_keys_from_path(dm_path_p, pDMNamePath, of_strlen(dm_path_p))) != OF_SUCCESS)
      {
         of_free (pDMNamePath);
         pDMResult->result_code = CM_JE_FAILURE;
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
               UCMJE_ERROR_DMPATH_INVALID, dm_path_p);
         *presult_p = pDMResult;
         return OF_FAILURE;
      }

      pDMNode = (struct cm_dm_data_element *)cmi_dm_get_node_by_namepath (pDMNamePath);
      if (unlikely (!pDMNode))
      {
         of_free (pDMNamePath);
         pDMResult->result_code = CM_JE_FAILURE;
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
               UCMJE_ERROR_DMPATH_INVALID, dm_path_p);
         *presult_p = pDMResult;
         return OF_FAILURE;
      }

      if(pDMNode->element_attrib.parent_trans_b == TRUE)
      {
         parent_trans_b = TRUE;
         of_free(pDMNamePath);
      }

      if(parent_trans_b != TRUE)
      {
#ifdef CM_ROLES_PERM_SUPPORT
         if (dm_path_p)
         {
            if(pDMNode->element_attrib.non_config_leaf_node_b != FALSE)     
            {
               parent_path_p = of_calloc(1, of_strlen(dm_path_p)+1);
               cm_remove_last_node_instance_from_dmpath(dm_path_p, parent_path_p);
               dm_path_p = parent_path_p;
            }

            if (return_value =
                  cm_verify_role_permissions (dm_path_p, config_request_p->admin_role,
                     config_request_p->sub_command_id) != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Permission Denied");
#ifdef CM_AUDIT_SUPPORT
               UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_PERMISSION_DENIED,config_request_p->admin_name,config_request_p->admin_role,
                     config_request_p->sub_command_id, dm_path_p, NULL,NULL);
#endif
               pDMResult->result_code = CM_JE_FAILURE;
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                     UCMJE_ERROR_PERMISSION_DENIED,  dm_path_p);
               *presult_p = pDMResult;
               if(!parent_path_p)
               { 
                  of_free(parent_path_p);
                  parent_path_p = NULL;
               }
               return OF_FAILURE;
            }

            if(!parent_path_p)
            {
               of_free(parent_path_p);
               parent_path_p = NULL;
            }
            dm_path_p = config_request_p->dm_path_p;
         }               
#endif
      }
   }
   switch (config_request_p->sub_command_id)
   {
      case CM_CMD_GET_DM_NODE_INFO:
         CM_JE_DEBUG_PRINT ("CM_CMD_GET_DM_NODE_INFO cmd");
         return_value = cmi_dm_get_node_info (dm_path_p, &node_info_p);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("CM_CMD_GET_DM_NODE_INFO Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_GET_DM_NODE_INFO_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         pDMResult->data.dm_info.count_ui = 1;
         pDMResult->data.dm_info.node_info_p = node_info_p;
         break;

      case CM_CMD_GET_DM_KEY_CHILD_INFO:
         CM_JE_DEBUG_PRINT ("CM_CMD_GET_DM_KEY_CHILD_INFO cmd");
         return_value = cmi_dm_get_key_child_info (dm_path_p, &node_info_p);
         if (return_value != OF_SUCCESS)
         {
            if(return_value == CM_COMMON_DM_JE_NO_KEY_FOR_TABLE)
            {
               CM_JE_DEBUG_PRINT ("CM_COMMON_NO_KEY_FOR_TABLE Failed");
               pDMResult->result_code = CM_JE_FAILURE;
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                     CM_COMMON_NO_KEY_FOR_TABLE,
                     dm_path_p);
            }
            else
            {
               CM_JE_DEBUG_PRINT ("CM_CMD_GET_DM_KEY_CHILD_INFO Failed");
               pDMResult->result_code = CM_JE_FAILURE;
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                     UCMJE_DMERROR_GET_DM_KEY_CHILD_INFO_FAILED,
                     dm_path_p);
            }
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         pDMResult->data.dm_info.count_ui = 1;
         pDMResult->data.dm_info.node_info_p = node_info_p;
         break;

      case CM_CMD_GET_DM_KEY_CHILDS:
         CM_JE_DEBUG_PRINT ("CM_CMD_GET_DM_KEY_CHILDS cmd");
         return_value = cmi_dm_get_key_child_nodes (dm_path_p, &node_info_array_p);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("CM_CMD_GET_DM_KEY_CHILDS Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_GET_DM_NODE_AND_CHILD_INFO_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         pDMResult->data.dm_info.count_ui = 0;
         pDMResult->data.dm_info.node_info_array_p = node_info_array_p;
         break;
      case CM_CMD_GET_DM_NODE_AND_CHILD_INFO:
         CM_JE_DEBUG_PRINT ("CM_CMD_GET_DM_NODE_AND_CHILD_INFO cmd");
         return_value = cmi_dm_get_node_and_child_info (dm_path_p, &node_info_p, &node_info_array_p);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("CM_CMD_GET_DM_NODE_AND_CHILD_INFO Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_GET_DM_NODE_AND_CHILD_INFO_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         pDMResult->data.dm_info.count_ui = 1;
         pDMResult->data.dm_info.node_info_p = node_info_p;
         pDMResult->data.dm_info.node_info_array_p = node_info_array_p;
         break;

      case CM_CMD_GET_DM_FIRST_NODE_INFO:
         CM_JE_DEBUG_PRINT ("CM_CMD_GET_DM_FIRST_NODE_INFO cmd");
         return_value = cmi_dm_get_first_node_info (dm_path_p,
               config_request_p->data.data_model.type,
               &node_info_p, &opaque_info_p, &opaque_info_length);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("CM_CMD_GET_DM_FIRST_NODE_INFO Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_GET_DM_FIRST_NODE_INFO_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
#ifdef CM_ROLES_PERM_SUPPORT
         /* If DMPath is NULL, call for Get Root Node*/
         if(dm_path_p == NULL)
         {
            /* Verifying the permissions to read Root Node*/
            if (return_value =
                  cm_verify_role_permissions (node_info_p->dm_path_p, config_request_p->admin_role,
                     config_request_p->sub_command_id) != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Permission Denied");
#ifdef CM_AUDIT_SUPPORT
               UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_PERMISSION_DENIED,config_request_p->admin_name,config_request_p->admin_role,
                     config_request_p->sub_command_id, node_info_p->dm_path_p, NULL,NULL);
#endif
               cmi_dm_free_node_info(node_info_p);
               of_free(opaque_info_p);
               pDMResult->result_code = CM_JE_FAILURE;
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                     UCMJE_ERROR_PERMISSION_DENIED,  
                     dm_path_p);
               *presult_p = pDMResult;
               return OF_FAILURE;
            }
         }
#endif
         pDMResult->result_code = CM_JE_SUCCESS;
         pDMResult->data.dm_info.count_ui = 1;
         pDMResult->data.dm_info.node_info_p = node_info_p;
         pDMResult->data.dm_info.opaque_info_p = opaque_info_p;
         pDMResult->data.dm_info.opaque_info_length = opaque_info_length;
         break;

      case CM_CMD_GET_DM_NEXT_NODE_INFO:
         CM_JE_DEBUG_PRINT ("CM_CMD_GET_DM_NEXT_NODE_INFO cmd");
         return_value = cmi_dm_get_next_node_info (dm_path_p,
               config_request_p->data.data_model.type,
               &node_info_p,
               &config_request_p->data.data_model.opaque_info_p,
               &config_request_p->data.data_model.opaque_info_length);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("CM_CMD_GET_DM_NEXT_NODE_INFO Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_GET_DM_NEXT_NODE_INFO_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
#ifdef CM_ROLES_PERM_SUPPORT
         if (return_value =
               cm_verify_role_permissions (node_info_p->dm_path_p, config_request_p->admin_role,
                  config_request_p->sub_command_id) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Verify Role Permission Failed");
            dm_path_p=node_info_p->dm_path_p;
#ifdef CM_AUDIT_SUPPORT
            UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_PERMISSION_DENIED,config_request_p->admin_name,config_request_p->admin_role,
                  config_request_p->sub_command_id, dm_path_p, NULL,NULL);
#endif
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_ERROR_PERMISSION_DENIED,  
                  dm_path_p);
            *presult_p = pDMResult;
            of_free(config_request_p->data.data_model.opaque_info_p);
            cmi_dm_free_node_info (node_info_p);
            return OF_FAILURE;
         }
#endif
         pDMResult->result_code = CM_JE_SUCCESS;
         pDMResult->data.dm_info.count_ui = 1;
         pDMResult->data.dm_info.node_info_p = node_info_p;
         pDMResult->data.dm_info.opaque_info_p =
            config_request_p->data.data_model.opaque_info_p;
         pDMResult->data.dm_info.opaque_info_length =
            config_request_p->data.data_model.opaque_info_length;
         break;

      case CM_CMD_GET_DM_NEXT_TABLE_ANCHOR_NODE_INFO:
         CM_JE_DEBUG_PRINT ("CM_CMD_GET_DM_NEXT_TABLE_ANCHOR_NODE_INFO");
         return_value = cmi_dm_get_next_table_n_anchor_node_info (dm_path_p,
               config_request_p->data.data_model.type,
               &node_info_p,
               &config_request_p->data.data_model.opaque_info_p,
               &config_request_p->data.data_model.opaque_info_length);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("CM_CMD_GET_DM_NEXT_NODE_INFO Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_GET_DM_NEXT_NODE_INFO_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
#ifdef CM_ROLES_PERM_SUPPORT
         if (cm_verify_role_permissions (node_info_p->dm_path_p, config_request_p->admin_role,
                  config_request_p->sub_command_id) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Verify Role Permission Failed");
            dm_path_p=node_info_p->dm_path_p;
#ifdef CM_AUDIT_SUPPORT
            UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_PERMISSION_DENIED,config_request_p->admin_name,config_request_p->admin_role,
                  config_request_p->sub_command_id, dm_path_p, NULL,NULL);
#endif
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_ERROR_PERMISSION_DENIED,  
                  dm_path_p);
            *presult_p = pDMResult;
            of_free(config_request_p->data.data_model.opaque_info_p);
            cmi_dm_free_node_info (node_info_p);
            return OF_FAILURE;
         }
#endif
         pDMResult->result_code = CM_JE_SUCCESS;
         pDMResult->data.dm_info.count_ui = 1;
         pDMResult->data.dm_info.node_info_p = node_info_p;
         pDMResult->data.dm_info.opaque_info_p =
            config_request_p->data.data_model.opaque_info_p;
         pDMResult->data.dm_info.opaque_info_length =
            config_request_p->data.data_model.opaque_info_length;
         break;
      case CM_CMD_GET_DM_KEYS_ARRAY:
         CM_JE_DEBUG_PRINT ("CM_CMD_GET_DM_KEYS_ARRAY cmd");
         return_value = cmi_dm_get_key_array_from_namepath (dm_path_p, &keys_array_p);
         if (return_value != OF_SUCCESS)
         {
            if(return_value == OF_FAILURE)
            {
              return_value = UCMJE_DMERROR_GET_DM_KEYS_ARRAY_FAILED;
            }
            
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  return_value, dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         pDMResult->data.dm_info.dm_node_nv_pairs_p = keys_array_p;
         break;

      case CM_CMD_GET_DM_INSTANCE_MAPLIST_BY_NAMEPATH:
         CM_JE_DEBUG_PRINT ("CM_CMD_GET_DM_INSTANCE_MAPLIST_BY_NAMEPATH cmd");
         return_value = cmi_dm_get_instance_map_entry_list_using_name_path (dm_path_p,
               &count_ui, &pMap);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT
               ("CM_CMD_GET_DM_INSTANCE_MAPLIST_BY_NAMEPATH Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_GET_DM_INSTANCE_MAPLIST_BY_NAMEPATH_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            CM_JE_DEBUG_PRINT
               ("cmi_dm_get_instance_map_entry_list_using_name_path Failed");
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         pDMResult->data.dm_info.count_ui = count_ui;
         pDMResult->data.dm_info.instance_map_list_p = pMap;
         break;

      case CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEY:
         CM_JE_DEBUG_PRINT ("CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEY");
         bInstExists=cmi_dm_is_instance_map_entry_using_key_and_name_path (dm_path_p,
               config_request_p->data.data_model.opaque_info_p,
               config_request_p->data.data_model.type);
         if (bInstExists ==   FALSE)
         {
            CM_JE_DEBUG_PRINT
               ("CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEY Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            pDMResult->result.success.reserved = FALSE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEY_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         pDMResult->result.success.reserved = TRUE;
         break;


      case CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEYARRAY:
         CM_JE_DEBUG_PRINT ("CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEYARRAY");
         bInstExists=cmi_dm_is_instance_map_entry_using_key_array_and_name_path (dm_path_p,
               &(config_request_p->data.nv_pair_array));
         if (bInstExists ==   FALSE)
         {
            CM_JE_DEBUG_PRINT
               ("CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEYARRAY Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            pDMResult->result.success.reserved = FALSE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEY_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         pDMResult->result.success.reserved = TRUE;
         break;

      case CM_CMD_IS_DM_CHILD_NODE:
         CM_JE_DEBUG_PRINT ("CM_CMD_IS_DM_CHILD_NODE");
         {
            unsigned char bIsChild;
            bIsChild=cmi_dm_is_child_element(dm_path_p,
                  (char *) config_request_p->data.data_model.opaque_info_p);
            if( bIsChild == FALSE)
            {
               CM_JE_DEBUG_PRINT ("CM_CMD_IS_DM_CHILD_NODE  Failed");
               pDMResult->result_code = CM_JE_FAILURE;
               pDMResult->result.success.reserved = FALSE;
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                     UCMJE_DMERROR_IS_DM_CHILD_NODE_FAILED,
                     dm_path_p);
               *presult_p = pDMResult;
               return OF_FAILURE;
            }
            pDMResult->result_code = CM_JE_SUCCESS;
            pDMResult->result.success.reserved = TRUE;
         }
         break;

      case CM_CMD_GET_SAVE_FILE_NAME:
         CM_JE_DEBUG_PRINT ("CM_CMD_GET_SAVE_FILE_NAME");
         return_value=cmi_dm_get_file_name(dm_path_p, &file_name_p);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT("cmi_dm_get_file_name Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_GET_SAVE_FILE_NAME_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         pDMResult->data.dm_info.opaque_info_p= file_name_p;
         pDMResult->data.dm_info.opaque_info_length=of_strlen(file_name_p);
         break;

      case CM_CMD_GET_SAVE_DIRECTORY_NAME:
         CM_JE_DEBUG_PRINT ("CM_CMD_GET_SAVE_DIRECTORY_NAME");
         return_value=cmi_dm_get_directory_name(dm_path_p, &directory_name_p);
         if(return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT("cmi_dm_get_file_name Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_GET_SAVE_DIRECTORY_NAME_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         pDMResult->data.dm_info.opaque_info_p= directory_name_p;
         pDMResult->data.dm_info.opaque_info_length=of_strlen(directory_name_p);
         break;
      case CM_CMD_GET_DM_CHILD_NODE_INFO:
      case CM_CMD_GET_DM_CHILD_NODE_COUNT :
         break;

      case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS:
         CM_JE_DEBUG_PRINT("CM_CMD_GET_DM_NODE_ROLE_ATTRIBS command");
#ifdef CM_ROLES_PERM_SUPPORT
         return_value= cmi_dm_get_role_permissions(dm_path_p,
               config_request_p->admin_role, (struct cm_dm_role_permission *)
               &role_perm_ary_p);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT("cmi_dm_get_role_permissions Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_GET_DM_NODE_ROLE_ATTRIBS_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         pDMResult->data.dm_info.opaque_info_p=(void*)role_perm_ary_p;
         break;
#else
         pDMResult->result_code = CM_JE_FAILURE;
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
               UCMJE_DMERROR_GET_DM_NODE_ROLE_ATTRIBS_FAILED,
               dm_path_p);
         *presult_p = pDMResult;
         return OF_FAILURE;
#endif

      case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
         CM_JE_DEBUG_PRINT("CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE command");
#ifdef CM_ROLES_PERM_SUPPORT
         return_value= cmi_dm_get_permissions_by_role(dm_path_p,
               config_request_p->admin_role, (struct cm_dm_role_permission *)
               config_request_p->data.data_model.opaque_info_p);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT("cmi_dm_get_role_permissionsByRole Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         pDMResult->data.dm_info.opaque_info_p=config_request_p->data.data_model.opaque_info_p;
         break;
#else
         pDMResult->result_code = CM_JE_FAILURE;
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
               UCMJE_DMERROR_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE_FAILED,
               dm_path_p);
         *presult_p = pDMResult;
         return OF_FAILURE;
#endif

      case CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
         CM_JE_DEBUG_PRINT("CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE command");
#ifdef CM_ROLES_PERM_SUPPORT
         if (!of_strcmp(CM_VORTIQA_SUPERUSER_ROLE, (char*)config_request_p->data.data_model.opaque_info_p))
            //  if (!of_strcmp(CM_DM_DEFAULT_ROLE, (char*)config_request_p->data.data_model.opaque_info_p))
         {
            CM_JE_DEBUG_PRINT("cmi_dm_del_permissions_by_role Super Admin role can't be deleted");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         return_value= cmi_dm_set_role_permissions(dm_path_p,config_request_p->admin_role,
               (struct cm_dm_role_permission *)
               config_request_p->data.data_model.opaque_info_p);
         if(return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT("cmi_dm_set_role_permissions Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE_FAILED,
                  dm_path_p);

            if(return_value == CM_DM_ERR_INVALID_INPUTS)
            {
               UCMJE_MEMALLOC_AND_COPY_RESULTSTRING_INTO_UCMRESULT (pDMResult,
                     "Invalid input");
            }

            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         break;
#else
         pDMResult->result_code = CM_JE_FAILURE;
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
               UCMJE_DMERROR_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE_FAILED,
               dm_path_p);
         *presult_p = pDMResult;
         return OF_FAILURE;
#endif

      case CM_CMD_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
#ifdef CM_ROLES_PERM_SUPPORT
         if (!of_strcmp(CM_VORTIQA_SUPERUSER_ROLE, (char*)config_request_p->data.data_model.opaque_info_p))
            //  if (!of_strcmp(CM_DM_DEFAULT_ROLE, (char*)config_request_p->data.data_model.opaque_info_p))
         {
            CM_JE_DEBUG_PRINT("cmi_dm_del_permissions_by_role Super Admin role can't be deleted");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         return_value= cmi_dm_del_permissions_by_role(dm_path_p,  config_request_p->admin_role,
               (struct cm_dm_role_permission *)  config_request_p->data.data_model.opaque_info_p);
         if ( return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT("cmi_dm_del_permissions_by_role Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         break;
#else
         pDMResult->result_code = CM_JE_FAILURE;
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
               UCMJE_DMERROR_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE_FAILED,
               dm_path_p);
         *presult_p = pDMResult;
         return OF_FAILURE;
#endif

      case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS:
         CM_JE_DEBUG_PRINT("CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS command");
#ifdef CM_ROLES_PERM_SUPPORT
         return_value = cmi_dm_get_instance_role_permissions(dm_path_p,
               config_request_p->admin_role,&role_perm_ary_p);
         if (return_value!= OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT("cmi_dm_get_instance_role_permissions Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_GET_DM_INSTANCE_ROLE_ATTRIBS_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         pDMResult->data.dm_info.opaque_info_p=(void *)role_perm_ary_p;
         break;
#else
         pDMResult->result_code = CM_JE_FAILURE;
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
               UCMJE_DMERROR_GET_DM_INSTANCE_ROLE_ATTRIBS_FAILED,
               dm_path_p);
         *presult_p = pDMResult;
         return OF_FAILURE;
#endif

      case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
         CM_JE_DEBUG_PRINT("CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE command");
#ifdef CM_ROLES_PERM_SUPPORT
         return_value = cmi_dm_get_instance_role_permissions_by_role(dm_path_p,
               config_request_p->admin_role,
               (struct cm_dm_role_permission *)
               config_request_p->data.data_model.opaque_info_p);
         if (return_value!= OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT("ucmdm_GetInstancePermissionsByRole Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_GET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         pDMResult->data.dm_info.opaque_info_p=config_request_p->data.data_model.opaque_info_p;
         break;
#else
         pDMResult->result_code = CM_JE_FAILURE;
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
               UCMJE_DMERROR_GET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE_FAILED,
               dm_path_p);
         *presult_p = pDMResult;
         return OF_FAILURE;
#endif

      case CM_CMD_SET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
         CM_JE_DEBUG_PRINT("CM_CMD_SET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE command");
#ifdef CM_ROLES_PERM_SUPPORT
         return_value= cmi_dm_set_instance_role_permissions(dm_path_p,  config_request_p->admin_role,
               (struct cm_dm_role_permission *)
               config_request_p->data.data_model.opaque_info_p);
         if ( return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT("ucmdm_SetInstancePermissionsByRole Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_SET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         break;
#else
         pDMResult->result_code = CM_JE_FAILURE;
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
               UCMJE_DMERROR_SET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE_FAILED,
               dm_path_p);
         *presult_p = pDMResult;
         return OF_FAILURE;
#endif

      case CM_CMD_DEL_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
         CM_JE_DEBUG_PRINT("CM_CMD_DEL_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE command");
#ifdef CM_ROLES_PERM_SUPPORT
         return_value= cmi_dm_del_instance_permissions_by_role(dm_path_p,
               config_request_p->admin_role,
               (struct cm_dm_role_permission *)
               config_request_p->data.data_model.opaque_info_p);
         if(return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT("ucmdm_DelInstancePermissinByRole Failed");
            pDMResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
                  UCMJE_DMERROR_DEL_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE_FAILED,
                  dm_path_p);
            *presult_p = pDMResult;
            return OF_FAILURE;
         }
         pDMResult->result_code = CM_JE_SUCCESS;
         break;
#else
         pDMResult->result_code = CM_JE_FAILURE;
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
               UCMJE_DMERROR_DEL_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE_FAILED,
               dm_path_p);
         *presult_p = pDMResult;
         return OF_FAILURE;
#endif

      case CM_CMD_GET_NEW_DM_INSTANCE_ID:
      case CM_CMD_ADD_DM_INSTANCE_MAP:
      case CM_CMD_DEL_DM_INSTANCE_MAP:
      default:
         CM_JE_DEBUG_PRINT ("unknown  Command");
         pDMResult->result_code = CM_JE_FAILURE;
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pDMResult,
               UCMJE_DMERROR_UNKNOWN_DM_COMMAND,
               dm_path_p);
         *presult_p = pDMResult;
         return OF_FAILURE;
   }

   if (return_value == OF_SUCCESS)
   {
      pDMResult->result_code = CM_JE_SUCCESS;
   }

   /* Free Input Key Array */
   *presult_p = pDMResult;
   return OF_SUCCESS;
}

/**
  \ingroup JEAPI
  This API Frames Load/Save/Factory-Reset request and sends to Load and Save
  Module by establishing transport channel.

  <b>Input paramameters: </b>\n
  <b><i> config_request_p: </i></b> pointer to Configuration Request sent by Management
  Engines.

  <b>Output Parameters: </b>
  <b><i>pUCMResult:</i></b> double pointer to UCMResult 
  In Success case it contains data for Get Requests
  In Failure case error information \n

  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cm_je_ldsv_handler
 ** Description   : This API Frames Load/Save/Factory-Reset request and sends to 
 Load and Save Module by establishing transport channel.
 ** Input params  : config_request_p - pointer to Configuration Request sent by Management
 **                              Engines.
 ** Output params : result_p - double pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cm_je_ldsv_handler (struct je_config_request * config_request_p,
      struct cm_result ** presult_p)
{
   struct cm_result *pLdsvResult = NULL;
   uint32_t return_value = 0;
   uint32_t ulReqLength = 0;
   char *pRequestBuf;
   char *pTmpRequest;
   char *dm_path_p;
   char uiDMPathLen = 0;
   uint32_t mgmt_engine_id=0; 
   struct cm_tnsprt_channel *pLdsvTransChannel = NULL;
   struct cm_command Command;
#ifdef CM_ROLES_PERM_SUPPORT
   char dmpath_a[6] = CM_DM_ROOT_NODE_PATH;
#endif
#ifdef CM_VERSION_SUPPORT
   uint64_t NewCfgVer = 0;
#ifdef  CM_EVENT_NOTIFMGR_SUPPORT
   struct cm_evmgr_event Event;
   time_t t;
#endif
#endif

   CM_JE_DEBUG_PRINT ("Entered");
   if (config_request_p->command_id != CM_COMMAND_LDSV_REQUEST)
   {
      CM_JE_DEBUG_PRINT ("Not a Ldsv  Request");
      return OF_FAILURE;
   }

   mgmt_engine_id = config_request_p->mgmt_engine_id;

   CM_INC_STATS_JE(JE_LDSVREQ_RECIEVES);

   pLdsvResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (pLdsvResult == NULL)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation failed for result structure");
      return OF_FAILURE;
   }

   dm_path_p = config_request_p->dm_path_p;

#ifdef CM_ROLES_PERM_SUPPORT
   if (!dm_path_p)
   {
      dmpath_a[3]='\0';
      dm_path_p=&dmpath_a[0];
   }
   if (return_value =  cm_verify_role_permissions (dm_path_p, config_request_p->admin_role,
            config_request_p->sub_command_id) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Permission Denied");
#ifdef CM_AUDIT_SUPPORT
      UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_PERMISSION_DENIED,config_request_p->admin_name,config_request_p->admin_role,
            config_request_p->sub_command_id, dm_path_p, NULL,NULL);
#endif
      pLdsvResult->result_code = CM_JE_FAILURE;
      if (dm_path_p)
      {
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pLdsvResult,
               UCMJE_ERROR_PERMISSION_DENIED,  dm_path_p);
      }
      else
      {
         pLdsvResult->result.error.error_code = UCMJE_ERROR_PERMISSION_DENIED;
      }
      *presult_p = pLdsvResult;
      return OF_FAILURE;
   }
#endif
   pLdsvTransChannel =
      (struct cm_tnsprt_channel *) cmje_establish_tnsprtchannel_with_ldsv ();
#ifdef CM_LDSV_SUPPORT
   if(bLdsvInitialized_g == FALSE)
   {
      if (pLdsvTransChannel == NULL)
      {
         while(pLdsvTransChannel == NULL)
         {
            of_sleep_ms(10);
            /*JE keep on tries for LDSV to establish a Connection*/
            /* Because default configuration needs to be loaded from LDSV*/
            pLdsvTransChannel =
               (struct cm_tnsprt_channel *) cmje_establish_tnsprtchannel_with_ldsv ();
         }
         bLdsvInitialized_g = TRUE;
      }
      else
      {
         bLdsvInitialized_g = TRUE;
      }
   }
#endif
   if (pLdsvTransChannel == NULL)
   {
      CM_JE_DEBUG_PRINT ("Establishing Transport Channel with Ldsv failed");
      pLdsvResult->result_code = CM_JE_FAILURE;
      pLdsvResult->result.error.error_code =
         UCMJE_ERROR_CREATE_TRANSCHANNEL_WTH_LDSV;
      if (dm_path_p)
      {
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pLdsvResult,
               UCMJE_ERROR_CREATE_TRANSCHANNEL_WTH_LDSV,
               dm_path_p);
      }
      *presult_p = pLdsvResult;
      of_free (pLdsvTransChannel);
      return OF_FAILURE;
   }

   if (dm_path_p)
   {
      CM_JE_DEBUG_PRINT ("dm_path_p is %s", config_request_p->dm_path_p);
      uiDMPathLen = of_strlen (config_request_p->dm_path_p);
   }

   /* sizeof lengh field + sub command length +
    *  dm path length field + DM Path Length + version*/   
   ulReqLength =
      uiOSIntSize_g + uiOSIntSize_g + uiOSIntSize_g + uiDMPathLen + uiInt64Size_g;

   pRequestBuf = (char *) of_calloc (1, ulReqLength);
   if (!pRequestBuf)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Request Buf");
      cmje_terminate_tnsprtchannel_with_ldsv (pLdsvTransChannel);

      pLdsvResult->result_code = CM_JE_FAILURE;
      pLdsvResult->result.error.error_code =
         UCMJE_ERROR_MEMORY_ALLOCATION_FAILED;
      if (dm_path_p)
      {
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pLdsvResult,
               UCMJE_ERROR_MEMORY_ALLOCATION_FAILED,
               dm_path_p);
      }
      *presult_p = pLdsvResult;
      return OF_FAILURE;
   }

   pTmpRequest = pRequestBuf;
   pTmpRequest = (char *)of_mbuf_put_32 (pTmpRequest, ulReqLength);

   switch (config_request_p->sub_command_id)
   {
      case CM_CMD_LOAD_CONFIG:
         CM_JE_DEBUG_PRINT ("Load Config");
         if(bLoadInProgress_g ==TRUE)
         {
            CM_JE_DEBUG_PRINT ("Load Configuration is already updated");
            pLdsvResult->result_code = CM_JE_FAILURE;
            if (dm_path_p)
            {
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pLdsvResult,
                     UCMJE_ERROR_LOAD_IS_IN_PROGRESS,
                     dm_path_p);
            }
            else
            {
               pLdsvResult->result.error.error_code = UCMJE_ERROR_LOAD_IS_IN_PROGRESS;
            }
            *presult_p = pLdsvResult;
            cmje_terminate_tnsprtchannel_with_ldsv (pLdsvTransChannel);
            of_free (pRequestBuf);
            return OF_FAILURE;
         }
         pTmpRequest = (char *)of_mbuf_put_32 (pTmpRequest, CM_CMD_LOAD_CONFIG);
         break;

      case CM_CMD_SAVE_CONFIG:
         CM_JE_DEBUG_PRINT ("Save Config");
         if(bSaveInProgress_g ==TRUE)
         {
            CM_JE_DEBUG_PRINT ("Save Configuration is already updated");
            pLdsvResult->result_code = CM_JE_FAILURE;
            if (dm_path_p)
            {
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pLdsvResult,
                     UCMJE_ERROR_SAVE_IS_IN_PROGRESS,
                     dm_path_p);
            }
            else
            {
               pLdsvResult->result.error.error_code = UCMJE_ERROR_SAVE_IS_IN_PROGRESS;
            }
            *presult_p = pLdsvResult;
            cmje_terminate_tnsprtchannel_with_ldsv (pLdsvTransChannel);
            of_free (pRequestBuf);
            return OF_FAILURE;
         }
         if( run_time_version_g == save_version_g )
         {
            CM_JE_DEBUG_PRINT ("Save Configuration is already updated");
            pLdsvResult->result_code = CM_JE_FAILURE;
            if (dm_path_p)
            {
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pLdsvResult,
                     UCMJE_ERROR_SAVE_CONFIG_IS_ALREADY_UPDATED,
                     dm_path_p);
            }
            else
            {
               pLdsvResult->result.error.error_code = UCMJE_ERROR_SAVE_CONFIG_IS_ALREADY_UPDATED;
            }
            *presult_p = pLdsvResult;
            cmje_terminate_tnsprtchannel_with_ldsv (pLdsvTransChannel);
            of_free (pRequestBuf);
            return OF_FAILURE;
         }
         pTmpRequest = (char *)of_mbuf_put_32 (pTmpRequest, CM_CMD_SAVE_CONFIG);
         break;

      case CM_CMD_RESET_TO_FACTORY_DEFAULTS:
         CM_JE_DEBUG_PRINT ("Factory Reset");
         return_value = cmje_delete_subtree (config_request_p->dm_path_p, NULL, pLdsvResult,
               DEL_RECORD,EXEC_COMPLETE_CMD,mgmt_engine_id);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("cmje_delete_subtree failed");
            pLdsvResult->result_code = CM_JE_FAILURE;
            *presult_p = pLdsvResult;
            cmje_terminate_tnsprtchannel_with_ldsv (pLdsvTransChannel);
            of_free (pRequestBuf);
            return OF_FAILURE;
         }

#ifdef CM_VERSION_SUPPORT
         Command.command_id=CM_CMD_RESET_TO_FACTORY_DEFAULTS;
         if (config_request_p->dm_path_p)
         {
            Command.dm_path_p=(char *)of_calloc(1,of_strlen(config_request_p->dm_path_p)+1);
            of_memcpy(Command.dm_path_p, config_request_p->dm_path_p, of_strlen(config_request_p->dm_path_p));
         }
         Command.nv_pair_array.count_ui=0;
         Command.nv_pair_array.nv_pairs=NULL;

         if ((return_value = cm_vh_create_new_config_version (&NewCfgVer)) != OF_SUCCESS)
         {
            pLdsvResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (pLdsvResult, UCMJE_ERROR_GET_NEWCONFIGVERSION_FAILED, dm_path_p);
            *presult_p = pLdsvResult;
            cmje_terminate_tnsprtchannel_with_ldsv (pLdsvTransChannel);
            of_free (pRequestBuf);
            return OF_FAILURE;
         }
         if ((return_value =
                  cm_vh_add_command_to_config_version (NewCfgVer,
                     &Command)) != OF_SUCCESS)
         {
            pLdsvResult->result_code = CM_JE_FAILURE;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (pLdsvResult, UCMJE_ERROR_ADDCMD_TO_CONFIGVERSION_FAILED, dm_path_p);
            *presult_p = pLdsvResult;
            cmje_terminate_tnsprtchannel_with_ldsv (pLdsvTransChannel);
            of_free (pRequestBuf);
            return OF_FAILURE;
         }

#ifdef CM_EVENT_NOTIFMGR_SUPPORT
         if (NewCfgVer > 0) 
         {
            CM_JE_DEBUG_PRINT (" ****NewCfgVer = %llx", NewCfgVer);
            Event.mgmt_engine_id = config_request_p->mgmt_engine_id;
            Event.date_time = time (&t);
            Event.event_type = CM_CONFIG_VERSION_UPDATE_EVENT;
            Event.Event.version_change.version = NewCfgVer;
            if ((return_value = cm_evmgr_generate_event (&Event)) != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Failed to Notify Event.");
               //pLdsvResult->result.error.error_code =
               //  UCMJE_ERROR_NOTIFICATION_CONFIGUPDATE_FAILED;
               //return
               //OF_FAILURE;
            }
         }
#endif  /*CM_EVENT_NOTIFMGR_SUPPORT*/
#endif /* CM_VERSION_SUPPORT */

         pTmpRequest = (char *)of_mbuf_put_32 (pTmpRequest, CM_CMD_RESET_TO_FACTORY_DEFAULTS);
         break;

      case CM_CMD_FLUSH_CONFIG:
         CM_JE_DEBUG_PRINT ("Flush ");
         return_value = cmje_delete_subtree (config_request_p->dm_path_p, NULL, pLdsvResult,
               DEL_RECORD,EXEC_COMPLETE_CMD,mgmt_engine_id);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("cmje_delete_subtree failed");
            pLdsvResult->result_code = CM_JE_FAILURE;
            *presult_p = pLdsvResult;
            cmje_terminate_tnsprtchannel_with_ldsv (pLdsvTransChannel);
            of_free (pRequestBuf);
            return OF_FAILURE;
         }

         /*
          ** Implementation of HA/Config Sync for configuration from Master if HA is enabled.
          **
          */
         pLdsvResult->result_code = CM_JE_SUCCESS;
         *presult_p = pLdsvResult;
         cmje_terminate_tnsprtchannel_with_ldsv (pLdsvTransChannel);
         of_free (pRequestBuf);
         return OF_SUCCESS;

      default:
         CM_JE_DEBUG_PRINT ("unknown  Sub Command for Ldsv");
         pLdsvResult->result_code = CM_JE_FAILURE;
         if (dm_path_p)
         {
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pLdsvResult,
                  UCMJE_ERROR_UNKNOWN_LDSV_SUB_COMMAND,
                  dm_path_p);
         }
         else
         {
            pLdsvResult->result.error.error_code = UCMJE_ERROR_UNKNOWN_LDSV_SUB_COMMAND;
         }

         *presult_p = pLdsvResult;
         cmje_terminate_tnsprtchannel_with_ldsv (pLdsvTransChannel);
         of_free (pRequestBuf);
         return OF_FAILURE;
   }

   pTmpRequest = (char *)of_mbuf_put_32 (pTmpRequest, uiDMPathLen);
   if ((config_request_p->dm_path_p) || (uiDMPathLen > 0))
   {
      CM_JE_DEBUG_PRINT ("dm_path_p is %s", config_request_p->dm_path_p);
      of_memcpy (pTmpRequest, config_request_p->dm_path_p, uiDMPathLen);
      pTmpRequest += uiDMPathLen;
   }
   CM_JE_DEBUG_PRINT ("Sending version Number %llx", run_time_version_g);
   pTmpRequest = (char *)of_mbuf_put_64 (pTmpRequest, run_time_version_g);
   if ((return_value = cm_socket_send (pLdsvTransChannel->sock_fd_i, pRequestBuf,
               ulReqLength, 0)) <= 0)
   {
      perror ("JE-SocketSend to Ldsv:");
      CM_JE_DEBUG_PRINT ("SocketSend Failed  to ldsv with errno = %d RetVal=%d",
            errno, return_value);
      pLdsvResult->result_code = CM_JE_FAILURE;
      if (dm_path_p)
      {
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pLdsvResult,
               UCMJE_ERROR_SEND_TO_LDSV_FAILED,
               dm_path_p);
      }
      else
      {
         pLdsvResult->result.error.error_code = UCMJE_ERROR_SEND_TO_LDSV_FAILED;
      }
      cmje_terminate_tnsprtchannel_with_ldsv (pLdsvTransChannel);
      of_free (pRequestBuf);
      *presult_p = pLdsvResult;
      return OF_FAILURE;
   }

   pLdsvResult->result_code = CM_JE_SUCCESS;
   cmje_terminate_tnsprtchannel_with_ldsv (pLdsvTransChannel);
   of_free (pRequestBuf);
   *presult_p = pLdsvResult;
   return OF_SUCCESS;
}

/**
  \ingroup JEAPI
  This API sets Transport Channel properties like inactivity time out value etc. 
  for specific transport channel.

  <b>Input paramameters: </b>\n
  <b><i> config_request_p: </i></b> pointer to Configuration Request sent by Management
  Engines.
  <b><i> tnsprt_channel_p: </i></b> pointer to Transport Channel

  <b>Output Parameters: </b>
  <b><i>pUCMResult:</i></b> double pointer to UCMResult 
  In Success case it contains data for Get Requests
  In Failure case error information \n

  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cm_tnsprt_channel_properties_handler 
 ** Description   : This API Sets Transport Channel Properties like Inactivity
 Time out value etc. 
 ** Input params  : config_request_p - pointer to Configuration Request sent by Management
 **                              Engines.
 **               : tnsprt_channel_p - pointer to transport channel.
 ** Output params : result_p - double pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                 OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cm_tnsprt_channel_properties_handler (struct je_config_request * config_request_p,
      struct cmje_tnsprtchannel_info * tnsprt_channel_p,
      struct cm_result ** presult_p)
{
   struct cm_result *xtn_entry_result_p = NULL;

   CM_JE_DEBUG_PRINT ("Entered");

   if(!presult_p)
   {
      CM_JE_DEBUG_PRINT ("presult_p NULL");
      return OF_FAILURE;
   }
   *presult_p=NULL;

   if (config_request_p->command_id != CM_COMMAND_SET_TRANSPORT_ATTRIBS)
   {
      CM_JE_DEBUG_PRINT ("Not a Trans Channel Set properties  Request");
      return OF_FAILURE;
   }
   xtn_entry_result_p = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (xtn_entry_result_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation failed for result structure");
      return OF_FAILURE;
   }

   switch (config_request_p->sub_command_id)
   {
      case CM_CMD_SET_INACTIVITY_TIMEOUT:
         CM_JE_DEBUG_PRINT ("Set Inactivity Time out command");
         //                      tnsprt_channel_p->max_inactive_time=config_request_p->  ;
         break;

      default:
         CM_JE_DEBUG_PRINT ("unknown Trasport Channel sub Command");
         xtn_entry_result_p->result.error.error_code = UCMJE_ERROR_UNKNOWN_TRCHANNEL_SUBCOMMAND;
         *presult_p=xtn_entry_result_p;
         return OF_FAILURE;
   }

   xtn_entry_result_p->result_code = CM_JE_SUCCESS;
   *presult_p = xtn_entry_result_p;
   return OF_SUCCESS;
}

int32_t cm_je_authentication_request_handler(struct je_config_request * config_request_p, struct cm_result ** presult_p)
{
   struct cm_role_info role_info;
   struct cm_result   *auth_result_p;
   struct cm_array_of_nv_pair *pNvPairAry;
   unsigned char aUserName[CM_MAX_ADMINNAME_LEN + 1]={0};
   unsigned char aPassword[CM_MAX_ADMINNAME_LEN + 1]={0};
   int32_t uiNVPairCount=0 , ii=0, return_value=OF_FAILURE;

   CM_JE_DEBUG_PRINT ("Entered");

   if (config_request_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid input");
      return OF_FAILURE;
   }

   auth_result_p = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (auth_result_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation failed for result structure");
      return OF_FAILURE;
   }

   uiNVPairCount = config_request_p->data.nv_pair_array.count_ui;

   for (ii=0; ii<uiNVPairCount; ii++)
   {
      if (of_strcmp(config_request_p->data.nv_pair_array.nv_pairs[ii].name_p, "username") == 0)
      {
         of_strncpy(aUserName,config_request_p->data.nv_pair_array.nv_pairs[ii].value_p,config_request_p->data.nv_pair_array.nv_pairs[ii].value_length);
      }
      if(of_strcmp(config_request_p->data.nv_pair_array.nv_pairs[ii].name_p, "password") == 0)
      {
         of_strncpy(aPassword,config_request_p->data.nv_pair_array.nv_pairs[ii].value_p,config_request_p->data.nv_pair_array.nv_pairs[ii].value_length);
      }
   }  

#ifdef CM_ROLES_PERM_SUPPORT
   /*Authenticate username and password, copy role permission*/
   return_value = (pRegLoginAuthFunPtr)(aUserName,aPassword,&role_info);
   if(return_value != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Authentication Failed");
      auth_result_p->result_code = CM_JE_FAILURE;
      *presult_p = auth_result_p;
      return OF_FAILURE;
   }
#endif
   /*Memory Allocation for NVPair Array*/
   pNvPairAry = (struct cm_array_of_nv_pair *)of_calloc(1,sizeof(struct cm_array_of_nv_pair));
   if(pNvPairAry == NULL)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation for nv_pairs failed");
      return OF_FAILURE;
   }

   /*Memory Allocation for admin name and rolename*/
   pNvPairAry->nv_pairs = (struct cm_nv_pair *) of_calloc (2, sizeof (struct cm_nv_pair));
   if(pNvPairAry->nv_pairs == NULL)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation for NVPair Arrays failed");
      of_free(pNvPairAry);
      return OF_FAILURE;
   }
   pNvPairAry->count_ui = 2;

   /*Filling admin name into NVPair*/
   pNvPairAry->nv_pairs[0].name_length = of_strlen("adminname");
   if(pNvPairAry->nv_pairs[0].name_length)
   {    
      pNvPairAry->nv_pairs[0].name_p = of_calloc (1, pNvPairAry->nv_pairs[0].name_length + 1);  
      if (pNvPairAry->nv_pairs[0].name_p == NULL)
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation Failed for admin name Name");
         return OF_FAILURE;
      }
      of_strncpy (pNvPairAry->nv_pairs[0].name_p, "adminname", pNvPairAry->nv_pairs[0].name_length);
   }  

   pNvPairAry->nv_pairs[0].value_type = CM_DATA_TYPE_STRING;
   pNvPairAry->nv_pairs[0].value_length = of_strlen(role_info.admin_name);
   if (pNvPairAry->nv_pairs[0].value_length > 0)
   {
      pNvPairAry->nv_pairs[0].value_p = of_calloc (1, pNvPairAry->nv_pairs[0].value_length+1);
      if (pNvPairAry->nv_pairs[0].value_p == NULL)
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation Failed for admin name Value");
         return OF_FAILURE;
      }
      of_strncpy (pNvPairAry->nv_pairs[0].value_p, role_info.admin_name, pNvPairAry->nv_pairs[0].value_length);
   }

   /*Filling adminrole into NVPair*/
   pNvPairAry->nv_pairs[1].name_length = of_strlen("adminrole");
   if(pNvPairAry->nv_pairs[1].name_length)
   {    
      pNvPairAry->nv_pairs[1].name_p = of_calloc (1, pNvPairAry->nv_pairs[1].name_length + 1);  
      if (pNvPairAry->nv_pairs[1].name_p == NULL)
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation Failed for adminrole Name");
         return OF_FAILURE;
      }
      of_strncpy (pNvPairAry->nv_pairs[1].name_p, "adminrole", pNvPairAry->nv_pairs[1].name_length);
   }

   pNvPairAry->nv_pairs[1].value_type = CM_DATA_TYPE_STRING;
   pNvPairAry->nv_pairs[1].value_length = of_strlen(role_info.admin_role);
   if (pNvPairAry->nv_pairs[1].value_length > 0)
   {
      pNvPairAry->nv_pairs[1].value_p = of_calloc (1, pNvPairAry->nv_pairs[1].value_length+1);
      if (pNvPairAry->nv_pairs[1].value_p == NULL)
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation Failed for adminrole Value");
         return OF_FAILURE;
      }
      of_strncpy (pNvPairAry->nv_pairs[1].value_p, role_info.admin_role, pNvPairAry->nv_pairs[1].value_length);
   }

   auth_result_p->result_code = CM_JE_SUCCESS;
   auth_result_p->sub_command_id = CM_SUBCOMMAND_AUTHENTICATE_USER;
   auth_result_p->data.je_data_p = (struct cm_array_of_nv_pair *)pNvPairAry;
   *presult_p = auth_result_p;
   return OF_SUCCESS;
}

int32_t cm_je_version_history_t_handler (struct je_config_request * config_request_p,
      struct cm_result ** presult_p)
{
   struct cm_result *pversionResult = NULL;
   uint32_t return_value = 0, uiCnt=0;
   char *dm_path_p = NULL;
   char dmpath_a[6] = CM_DM_ROOT_NODE_PATH;
   struct cm_version *version_list_p=NULL;
   struct cm_array_of_nv_pair *pnv_pair_array;

   if (config_request_p->command_id != CM_COMMAND_VERSION_HISTORY)
   {
      CM_JE_DEBUG_PRINT ("Not a Verison history_t Request");
      return OF_FAILURE;
   }

   CM_INC_STATS_JE(JE_VERHISTORYREQ_RECIEVES);
   pversionResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (pversionResult == NULL)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation failed for result structure");
      return OF_FAILURE;
   }

   dm_path_p = config_request_p->dm_path_p;
#ifdef CM_ROLES_PERM_SUPPORT
   if (!dm_path_p)
   {
#ifdef CM_MGMT_CARD_SUPPORT
      dmpath_a[4]='\0';
#else
      dmpath_a[3]='\0';
#endif
      dm_path_p=&dmpath_a[0];
   }
   if (return_value =  cm_verify_role_permissions (dm_path_p, config_request_p->admin_role,
            config_request_p->sub_command_id) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Permission Denied");
#ifdef CM_AUDIT_SUPPORT
      UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_PERMISSION_DENIED,config_request_p->admin_name,config_request_p->admin_role,
            config_request_p->sub_command_id, dm_path_p, NULL,NULL);
#endif
      pversionResult->result_code = CM_JE_FAILURE;
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pversionResult,
            UCMJE_ERROR_PERMISSION_DENIED,  dm_path_p);
      *presult_p = pversionResult;
      return OF_FAILURE;
   }
#endif

   switch (config_request_p->sub_command_id)
   {
      case CM_CMD_GET_DELTA_VERSION_HISTORY:
         CM_JE_DEBUG_PRINT ("CM_CMD_GET_DELTA_VERSION_HISTORY cmd");
         return_value = cm_vh_get_delta_version_history_t (config_request_p->data.version.version, 
               config_request_p->data.version.end_version, &uiCnt,
               &version_list_p);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("CM_CMD_GET_DELTA_VERSION_HISTORY Failed");
            pversionResult->result_code = return_value;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pversionResult,
                  UCMJE_DMERROR_GET_DM_NODE_INFO_FAILED,
                  dm_path_p);
            *presult_p = pversionResult;
            return OF_FAILURE;
         }
         pversionResult->result_code = CM_JE_SUCCESS;
         pversionResult->data.version.version_list_p = (struct cm_version *) version_list_p;
         pversionResult->data.version.count_ui = uiCnt;
         break;
      case CM_CMD_GET_VERSIONS:
         CM_JE_DEBUG_PRINT ("CM_CMD_GET_VERSIONS command");
         return_value=cm_vh_get_versions(&pnv_pair_array);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("CM_CMD_GET_VERSION Failed");
            pversionResult->result_code = return_value;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pversionResult,
                  UCMJE_DMERROR_GET_DM_NODE_INFO_FAILED,
                  dm_path_p);
            *presult_p = pversionResult;
            return OF_FAILURE;
         }
         pversionResult->data.je_data_p = (void *)pnv_pair_array;
         break;
      default:
         CM_JE_DEBUG_PRINT ("unknown  Command");
         pversionResult->result_code = CM_JE_FAILURE;
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pversionResult,
               UCMJE_DMERROR_UNKNOWN_DM_COMMAND,
               dm_path_p);
         *presult_p = pversionResult;
         return OF_FAILURE;
   }

   if (return_value == OF_SUCCESS)
   {
      pversionResult->result_code = CM_JE_SUCCESS;
   }

   /* Free Input Key Array */
   *presult_p = pversionResult;
   return OF_SUCCESS;
}

int32_t cm_je_rebootdevice_handler (struct je_config_request * config_request_p,
       struct cm_result ** presult_p)
{
 
        struct cm_result *pNotifyResult = NULL;
   pNotifyResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (pNotifyResult == NULL)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation failed for result structure");
      return OF_FAILURE;
   }

    if(system("cd /igateway ;sh reset.sh") == -1) 
    {
         CM_JE_DEBUG_PRINT("Failed to reboot the device ");
         pNotifyResult->result_code =  CM_JE_FAILURE;
	 *presult_p = pNotifyResult;
	 return OF_FAILURE;
    }
	
    pNotifyResult->result_code = CM_JE_SUCCESS;
   *presult_p = pNotifyResult;
    return OF_SUCCESS;
}

int32_t  cm_je_notification_handler (struct je_config_request * config_request_p,
      struct cm_result ** presult_p)
{
   struct cm_result *pNotifyResult = NULL;
   uint32_t return_value = 0;
   char *dm_path_p = NULL;
   char dmpath_a[6] = CM_DM_ROOT_NODE_PATH;

   CM_JE_DEBUG_PRINT ("Entered");
   if (config_request_p->command_id != CM_COMMAND_NOTIFICATION)
   {
      CM_JE_DEBUG_PRINT ("Not a Notification  Request");
      return OF_FAILURE;
   }

   CM_INC_STATS_JE(JE_NOTIFICATION_RECIEVES);
   pNotifyResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (pNotifyResult == NULL)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation failed for result structure");
      return OF_FAILURE;
   }

   dm_path_p = config_request_p->dm_path_p;
#ifdef CM_ROLES_PERM_SUPPORT
   if (!dm_path_p)
   {
      dm_path_p=dmpath_a;
   }
   if (return_value =  cm_verify_role_permissions (dm_path_p, config_request_p->admin_role,
            config_request_p->sub_command_id) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Permission Denied");
#ifdef CM_AUDIT_SUPPORT
      UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_PERMISSION_DENIED,config_request_p->admin_name,config_request_p->admin_role,
            config_request_p->sub_command_id, dm_path_p, NULL,NULL);
#endif
      pNotifyResult->result_code = CM_JE_FAILURE;
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pNotifyResult,
            UCMJE_ERROR_PERMISSION_DENIED,  dm_path_p);
      *presult_p = pNotifyResult;
      return OF_FAILURE;
   }
#endif

   switch (config_request_p->sub_command_id)
   {
      case CM_CMD_NOTIFY_JE:
         CM_JE_DEBUG_PRINT ("CM_CMD_NOTIFY_JE cmd");
         return_value = cm_je_read_notifications (config_request_p->mgmt_engine_id,config_request_p->data.Notify.event,
               config_request_p->data.Notify.pData, pNotifyResult);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("CM_CMD_NOTIFY_JE Failed");
            pNotifyResult->result_code = return_value;
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pNotifyResult,
                  UCMJE_DMERROR_GET_DM_NODE_INFO_FAILED,
                  dm_path_p);
            *presult_p = pNotifyResult;
            return OF_FAILURE;
         }
         pNotifyResult->result_code = CM_JE_SUCCESS;
         break;

      default:
         CM_JE_DEBUG_PRINT ("unknown  Command");
         pNotifyResult->result_code = CM_JE_FAILURE;
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pNotifyResult,
               UCMJE_DMERROR_UNKNOWN_DM_COMMAND,
               dm_path_p);
         *presult_p = pNotifyResult;
         return OF_FAILURE;
   }

   if (return_value == OF_SUCCESS)
   {
      pNotifyResult->result_code = CM_JE_SUCCESS;
   }

   /* Free Input Key Array */
   *presult_p = pNotifyResult;
   return OF_SUCCESS;
}

#endif
#endif
