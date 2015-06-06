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
 * File name: jecfgsess.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description: 
*/

#ifdef CM_SUPPORT
#ifdef CM_JE_SUPPORT

#include "jeincld.h"
#include "jeldef.h"
#include "jever.h"
#include "jelif.h"
#include "evntgdef.h"
#include "evntgif.h"
#include "dmlif.h"
//#include "ucmmesg.h"
#include "jesyslog.h"
#ifndef CM_MGMT_CARD_SUPPORT
//#include "platfrmdefines.h"
#endif

#define TRACE_SEVERE 4
#ifdef CM_TRACE_SUPPORT
#define Trace_1(format,...)     Trace(format, __VA_ARGS__)
#else
#define Trace_1(format,...)     
#endif
#define CM_DM_MAX_DEFAULT_ELEMENT_VALLEN 50
#define CM_JE_MAX_REFFERENCE_LEN 100
#define CM_JE_MAX_DMPATH_LEN 100

uint64_t run_time_version_g;
uint64_t save_version_g;

unsigned char bLoadInProgress_g;
unsigned char bSaveInProgress_g;

extern struct cm_dm_channel_callbacks *cm_channels_p[CM_MAX_CHANNEL_ID];
extern struct cm_dm_data_element RelativityNode;
UCMDllQ_t je_del_stack_list;
extern UCMDllQ_t UCMJEGloTransaction_g;
extern UCMDllQ_t cm_je_request_queue_g;
/******************************************************************************
 * * * * * * * * * * * * * * * FUNCTION PROTOTYPES * * * * * * * * * * * * * * 
 *****************************************************************************/

 int32_t je_sm_cfgsess_update_handler (struct je_config_session * config_session_p,
      struct je_config_request *
      config_request_p,
      struct cm_result * result_p);

 int32_t je_sm_cfgsess_commit_handler (struct je_config_session * config_session_p,
      struct je_config_request *
      config_request_p,
      struct cm_result * result_p);

 int32_t je_sm_cfgsess_revoke_handler (struct je_config_session * config_session_p,
      struct je_config_request *
      config_request_p,
      struct cm_result * result_p);

 int32_t je_sm_cfgsess_timeout_handler (struct je_config_session *
      config_session_p,
      struct je_config_request *
      config_request_p,
      struct cm_result * result_p);

 int32_t je_sm_cfgsess_get_handler (struct je_config_session * config_session_p,
      struct je_config_request *
      config_request_p, struct cm_result * result_p);

 int32_t je_validate_command (uint32_t command_id, char * dm_path_p,
      struct cm_array_of_nv_pair nv_pair_array,
      struct cm_result * result_p);

 int32_t je_verify_command_referrence (struct je_config_session *
      config_session_p,
      struct cm_result * result_p);
 
int32_t je_verify_referrence (UCMDllQ_t *pCfgList,
      struct cm_result * result_p);

 int32_t je_validate_attribute (struct cm_dm_data_element * attrib_p,
      struct cm_nv_pair * pNvPair,
      struct cm_result * result_p);

 int32_t je_check_session_mandatory_params (struct je_config_session *
      config_session_p, struct cm_result * result_p);

 int32_t je_execute_test_func_handler (struct je_config_session * config_session_p,
      struct cm_result * result_p);

 int32_t je_execute_command_test_func (char * dm_path_p,
      struct cm_array_of_nv_pair *
      pnv_pair_array,
      uint32_t command_id,
      unsigned char referrence_in_cache_b,
      struct cm_result * result_p,
      uint32_t mgmt_engine_id);

 int32_t je_execute_cbk_func_handler (struct je_config_session * config_session_p,
      uint16_t * pusCbkSuccessCnt,
      struct cm_result * result_p);

 int32_t je_execute_trans_commands (struct je_config_session *
      config_session_p,
      uint16_t * pusCbkSuccessCnt,
      struct cm_result * result_p);

 int32_t je_execute_command_cbk_func (char * dm_path_p,
      struct cm_array_of_nv_pair *
      pnv_pair_array, uint32_t command_id,
      struct cm_result * result_p,
      int32_t state,
      uint32_t mgmt_engine_id);

 int32_t je_execute_command( struct je_config_session *config_session_p,
      struct je_command_list *pInCommandList,
      uint32_t command_id,
      struct cm_result *result_p);

 int32_t je_execute_done_command( struct je_config_session *config_session_p,
      struct je_command_list *pInCommandList,
      uint32_t command_id,
      struct cm_result *result_p);

 int32_t je_start_transaction( struct je_command_list *pInCommandList,
      uint32_t command_id,
      struct cm_result *result_p,
      uint32_t mgmt_engine_id);

 int32_t DelInstances(struct je_config_session *config_session_p,
      struct cm_result * result_p);


 int32_t je_add_instance_map_entry (char * dm_path_p,
      struct cm_array_of_nv_pair * key_nv_pairs_p);

 int32_t je_check_mandatory_params(struct je_command_list * command_list_p,
      struct cm_result * result_p);

#ifdef CM_ROLES_PERM_SUPPORT
 int32_t cm_je_set_role_to_instance (char * dm_path_p,
      char * role_name_p,
      struct cm_array_of_nv_pair * key_nv_pair_p);
#endif

#ifdef CM_AUDIT_SUPPORT
 int32_t cm_generate_audit_log_message (struct je_config_session *
      config_session_p, struct cm_result * result_p);
#endif
 int32_t je_cfgsess_check_dup_values (struct je_command_list * command_list_p,
      struct cm_array_of_nv_pair *
      pnv_pair_array,
      uint32_t * puiReplacedId,
      uint32_t * puiCopiedNvCnt);

 int32_t je_cfgsess_add_to_exist_commandlist (struct je_command_list *
      command_list_p,
      struct je_config_request *
      config_request_p);

 int32_t je_cfgsess_add_to_new_commandlist (struct je_command_list *
      command_list_p,
      struct je_config_request *
      config_request_p);

 int32_t je_set_nvpair_array_to_default_values (char * dm_path_p,
      struct cm_array_of_nv_pair *
      arr_nv_pair_p,
      struct cm_result * result_p);

 int32_t je_set_param_to_default (char * dm_path_p,
      struct cm_nv_pair * pNvPair,
      struct cm_result * result_p);

 int32_t je_delete_nvpair_from_commandlist (struct je_command_list *
      command_list_p,
      char * pDelNvPairName,
      struct cm_result * result_p);
 int32_t cm_remove_last_node_instance_from_dmpath(char *dm_path_p, char *parent_path_p);

 int32_t je_get_trans_rec(char * dm_path_p,void **xtn_rec);

 int32_t je_get_global_parent(char *dm_path_p, char *pGlobalParent);

 int32_t je_get_parent_path(char *dm_path_p, char *parent_path_p);

 int32_t je_cfgsess_check_key_values (struct je_command_list *command_list_p,
      struct cm_array_of_nv_pair * pnv_pair_array,
      uint32_t *puiReplacedId);

#ifdef CM_VERSION_SUPPORT
 int32_t cm_je_add_command_cache_to_config_version (struct je_config_session *
      config_session_p,
      uint64_t * pNewCfgVer,
      struct cm_result * result_p);

#endif
//int32_t UCMJEGetFriendlyNames(struct cm_array_of_nv_pair *arr_nv_pair_p,char* dm_path_p,struct cm_result * result_p);
/******************************************************************************
 * * * * * * * * * * * * * * * GLOBAL VARIABLES* * * * * * * * * * * * * * * * 
 *****************************************************************************/
JESMFuncPtr cm_je_state_machine_table[UCMJE_MAX_STATE][UCMJE_MAX_EVENT] = {
   {
      /*UPDATE*/ je_sm_cfgsess_update_handler,
      /*COMMIT*/ je_sm_cfgsess_commit_handler,
      /*REVOKE*/ je_sm_cfgsess_revoke_handler,
      /*TIMEOUT*/ je_sm_cfgsess_timeout_handler,
      /*GET*/ je_sm_cfgsess_get_handler,
   }
};

uint32_t uiJEsession_id_g = 0;

#ifdef CM_JE_STATS_SUPPORT
extern int32_t cm_je_global_stats_g[JE_STATS_MAX];
extern int32_t cm_je_session_stats_g[JE_SESSION_STATS_MAX];
#endif
/******************************************************************************
 * * * * * * * * * * * * * FUNCTION DEFINITIONS * * * * * * * * * * * * * * * * 
 *****************************************************************************/

/**
  \ingroup JEAPI
  This API reads Input Data and adds commands to Configuration Cache.
  See where this command fits into the existing command list.
  If there are two similar add commands, recent one will overwrite the older one.
  If there is a add command followed by delete command, both the commands will be
  deleted from the command list.
  if there is an add command followed by set command, the set command params
  will be added to add command itself.

  <b>Input paramameters: </b>\n
  <b><i> config_session_p: </i></b> Pointer to Configuration Session for which request
  belongs to 
  <b><i> config_request_p: </i></b> pointer to Configuration Request sent by Management
  Engines.

  <b>Output Parameters: </b>
  <b><i>pUCMResult:</i></b> pointer to UCMResult 
  In Success case it contains data for Get Requests
  In Failure case error information \n

  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */

/******************************************************************************
 ** Function Name : je_sm_cfgsess_update_handler
 ** Description   : This API reads Input Data and adds commands to Configuration Cache.
 ** Input params  : config_session_p - Pointer to Configuration Session
 **                 config_request_p - pointer to Configuration Request sent by Management
 **                              Engines.
 ** Output params : result_p - pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/

 int32_t je_sm_cfgsess_update_handler (struct je_config_session * config_session_p,
      struct je_config_request *
      config_request_p,
      struct cm_result * result_p)
{
   struct je_command_list *command_list_p = NULL;
   uint32_t return_value = 0;
   uint32_t uiNvCnt = 0;
   uint32_t uiPathLen = 0;
   char *dm_path_p = NULL;
   unsigned char bCmdFound = FALSE;
   unsigned char global_trans_b = FALSE;
   unsigned char parent_trans_b = FALSE;
   char *pCancelCmdName = NULL;
   char *parent_path_p = NULL;
   uint32_t mgmt_engine_id = 0;
   struct cm_dm_data_element *pDMNode, *pDMParentNode=NULL;
   UCMDllQNode_t *pDllQNode = NULL;


   CM_JE_DEBUG_PRINT ("Entered");
   if (config_session_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("config_session_p is NULL");
      return OF_FAILURE;
   }
   if (config_session_p->pCache == NULL)
   {
      CM_JE_DEBUG_PRINT ("Configuration Cache is not initialized");
      return OF_FAILURE;
   }
   if (config_request_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("command_p is NULL");
      return OF_FAILURE;
   }

   mgmt_engine_id = config_session_p->mgmt_engine_id;
   dm_path_p = config_request_p->dm_path_p;
   pDMParentNode = (struct cm_dm_data_element *)
      cmi_dm_get_dm_node_from_instance_path (dm_path_p, of_strlen (dm_path_p));
   if (!pDMParentNode)
   {
      CM_JE_DEBUG_PRINT ("pDMParentNode is NULL");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_GETNODE_BYDMPATH,
            dm_path_p);
      return OF_FAILURE;
   }

   uiPathLen = of_strlen(dm_path_p);
   parent_path_p = (char *)of_calloc(1,uiPathLen+1);

   cm_remove_last_node_instance_from_dmpath(dm_path_p,parent_path_p);

   if(pDMParentNode->element_attrib.parent_trans_b == 1)
   {
      parent_trans_b = TRUE;
   }

   if(parent_trans_b == FALSE)
   {
#if CM_ROLES_PERM_SUPPORT
      if (cm_verify_role_permissions (parent_path_p, config_request_p->admin_role,
               config_request_p->sub_command_id) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Permission Denied");
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
               UCMJE_ERROR_PERMISSION_DENIED,
               dm_path_p);
         of_free(parent_path_p);
         return OF_FAILURE;
      }
#endif 
   }

   of_free(parent_path_p);
   parent_path_p = NULL;

   if (return_value = je_validate_command (config_request_p->sub_command_id, config_request_p->dm_path_p,
            config_request_p->data.nv_pair_array,
            result_p) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Validation Failed");
      return UCMJE_ERROR_VALIDATION_FAILED;
   }

   if(pDMParentNode->element_attrib.global_trans_b == 1)
   {
      global_trans_b = TRUE;
   }

   switch (config_request_p->sub_command_id)
   {
      case CM_CMD_ADD_PARAMS:
         CM_JE_DEBUG_PRINT ("Add Command");
         CM_INC_STATS_JESESSION (JE_SESSION_ADD_PARAMS);

         if(pDMParentNode->element_attrib.non_config_leaf_node_b != FALSE &&
               pDMParentNode->element_attrib.non_config_leaf_node_b != TRUE_ADD)
         {
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (result_p, UCMJE_ERROR_PERMISSION_DENIED, dm_path_p);
            return OF_FAILURE;
         }

         /*    pInode = cmi_dm_get_inode_by_name_path (parent_path_p);
               if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
               {
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (result_p, UCMJE_ERROR_DMPATH_INVALID, dm_path_p);
               return OF_FAILURE;
               } */

         /* Verify Command Already present in Cache?? */
         CM_DLLQ_SCAN (&(config_session_p->pCache->add_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            if (!strcmp (command_list_p->command_p->dm_path_p, dm_path_p))
            {
               if (je_cfgsess_add_to_exist_commandlist (command_list_p, config_request_p) !=
                     OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Duplicate Node found");
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
                     (result_p, UCMJE_ERROR_DMPATH_INVALID, dm_path_p);
                  return OF_FAILURE;
               }
               bCmdFound = TRUE;
               break;
            }
         }

         CM_DLLQ_SCAN (&(config_session_p->pCache->xtn_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            if ((!strcmp (command_list_p->command_p->dm_path_p, dm_path_p)
                     && command_list_p->command_p->command_id == CM_CMD_ADD_PARAMS)
                  &&(command_list_p->exec_command_b != TRUE))
               // && (command_list_p->command_p->command_id == config_request_p->sub_command_id))
            {
               if (je_cfgsess_add_to_exist_commandlist (command_list_p, config_request_p) !=
                     OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Duplicate Node found");
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
                     (result_p, UCMJE_ERROR_DMPATH_INVALID, dm_path_p);
                  return OF_FAILURE;
               }
               bCmdFound = TRUE;
               break;
            }
         }

         if (bCmdFound == TRUE)
         {
            if((global_trans_b == TRUE || parent_trans_b == TRUE)
                  && command_list_p->state <= START_TRANSACTION)
            {
               if(je_start_transaction(command_list_p,
                        CM_CMD_ADD_PARAMS,
                        result_p,
                        mgmt_engine_id) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT("command  execution failed");
                  return OF_FAILURE;
               }
            }
            CM_JE_DEBUG_PRINT ("Command Succesfully Added to ADD Cache");
            break;
         }

         /* Allocate Memory for New Command List */
         command_list_p = of_calloc (1, sizeof (struct je_command_list));
         if (!command_list_p)
         {
            CM_JE_DEBUG_PRINT ("Memory Allocation for Command List Failed");
            return OF_FAILURE;
         }

         /* Add request to New Command List  */
         if (je_cfgsess_add_to_new_commandlist (command_list_p, config_request_p) !=
               OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Add to New Command List Failed");
            of_free (command_list_p);
            return OF_FAILURE;
         }

         command_list_p->command_p->command_id = CM_CMD_ADD_PARAMS;

         if(global_trans_b == TRUE || parent_trans_b == TRUE)
         {
            if(je_start_transaction(command_list_p,
                     CM_CMD_ADD_PARAMS,
                     result_p,
                     mgmt_engine_id) != OF_SUCCESS)
            {
               je_free_command_list (command_list_p);           
               CM_JE_DEBUG_PRINT("start transaction failed");
               return OF_FAILURE;
            }
         }

         /* Increment Command Count */
         config_session_p->pCache->no_of_commands++;

         /* Assign Sequuest ID to Command List */
         command_list_p->sequence_id = config_session_p->pCache->no_of_commands;


         /* Append New Command List to ADD DLLQ List */
         if(parent_trans_b == TRUE || global_trans_b == TRUE)
         {
            CM_DLLQ_APPEND_NODE (&(config_session_p->pCache->xtn_config_list),
                  &command_list_p->list_node);
            command_list_p->xtn_b = TRUE;
         }
         else
         {
            CM_DLLQ_APPEND_NODE (&(config_session_p->pCache->add_config_list),
                  &command_list_p->list_node);
         }
         CM_JE_DEBUG_PRINT ("Command Succesfully Added to ADD Cache");
         break;

      case CM_CMD_SET_PARAMS:
         CM_JE_DEBUG_PRINT ("Set Command");
         CM_INC_STATS_JESESSION (JE_SESSION_SET_PARAMS);

         /* Verify Command Already present in  ADD Cache?? */
         CM_DLLQ_SCAN (&(config_session_p->pCache->add_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            if (!strcmp (command_list_p->command_p->dm_path_p, config_request_p->dm_path_p))
            {
               if (je_cfgsess_add_to_exist_commandlist (command_list_p, config_request_p) !=
                     OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Adding to Existing Command List Failed");
                  /* Frame Error Message */
                  return OF_FAILURE;
               }
               bCmdFound = TRUE;
               break;
            }
         }
         if (bCmdFound == TRUE)
         {
            CM_JE_DEBUG_PRINT ("Command Succesfully Added to Add Cache");
            break;
         }
         CM_DLLQ_SCAN (&(config_session_p->pCache->delete_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            if (!strcmp (command_list_p->command_p->dm_path_p, config_request_p->dm_path_p))
            {
               if (je_cfgsess_add_to_exist_commandlist (command_list_p, config_request_p) !=
                     OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Adding to Existing DEL Command List Failed");
                  /* Frame Error Message */
                  return OF_FAILURE;
               }
               bCmdFound = TRUE;
               break;
            }
         }
         if (bCmdFound == TRUE)
         {
            CM_JE_DEBUG_PRINT ("Command Succesfully Added to Del Cache");
            break;
         }
         /* Verify Command Already present in  MODIFY Cache?? */
         CM_DLLQ_SCAN (&(config_session_p->pCache->modify_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            /* Replace Exisiting Name Value Pairs */
            if (!strcmp (command_list_p->command_p->dm_path_p, config_request_p->dm_path_p))
            {
               if (je_cfgsess_add_to_exist_commandlist (command_list_p, config_request_p) !=
                     OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT
                     ("Addint to Existing Modify Command LIst Failed");
                  /* Frame Error Message */
                  return OF_FAILURE;
               }
               bCmdFound = TRUE;
               break;
            }
         }

         if (bCmdFound == TRUE)
         {
            CM_JE_DEBUG_PRINT ("Command Succesfully Added to Modify Cache");
            break;
         }

         CM_DLLQ_SCAN (&(config_session_p->pCache->xtn_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            if ((!strcmp (command_list_p->command_p->dm_path_p, dm_path_p))
                  &&(command_list_p->exec_command_b != TRUE))
         //       && (command_list_p->command_p->command_id == config_request_p->sub_command_id))
            {
               if (je_cfgsess_add_to_exist_commandlist (command_list_p, config_request_p) !=
                     OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Duplicate Node found");
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
                     (result_p, UCMJE_ERROR_DMPATH_INVALID, dm_path_p);
                  return OF_FAILURE;
               }
               bCmdFound = TRUE;
               break;
            }
         }
         if (bCmdFound == TRUE)
         {
            if(global_trans_b == 1 && command_list_p->state <= START_TRANSACTION)
            {
               if(je_start_transaction(command_list_p,
                        CM_CMD_SET_PARAMS,
                        result_p,
                        mgmt_engine_id) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT("command  execution failed");
                  return OF_FAILURE;
               }
            }
            CM_JE_DEBUG_PRINT ("Command Succesfully Added to Modify Cache");
            break;
         }

         if(parent_trans_b != TRUE)
         {
#if CM_ROLES_PERM_SUPPORT
            if(pDMParentNode->element_attrib.non_config_leaf_node_b == TRUE_MOD_DEL ||
                  pDMParentNode->element_attrib.non_config_leaf_node_b == TRUE_ADD)
            {
               parent_path_p = (char *)of_calloc(1,uiPathLen+1);
               cm_remove_last_node_instance_from_dmpath(dm_path_p,parent_path_p);
               dm_path_p = parent_path_p;
            }  


            if (cm_verify_role_permissions (dm_path_p, config_request_p->admin_role,
                     config_request_p->sub_command_id) != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Permission Denied");
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                     UCMJE_ERROR_PERMISSION_DENIED,
                     dm_path_p);
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
#endif
         }

         /* Allocate Memory for New Command List */
         command_list_p = of_calloc (1, sizeof (struct je_command_list));
         if (!command_list_p)
         {
            CM_JE_DEBUG_PRINT ("Memory Allocation for Command List Failed");
            return OF_FAILURE;
         }

         /* Allocate Memory for New Command
          * */
         if (je_cfgsess_add_to_new_commandlist (command_list_p, config_request_p) !=
               OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Add to New Command List Failed");
            of_free (command_list_p);
            return OF_FAILURE;
         }

         command_list_p->command_p->command_id = CM_CMD_SET_PARAMS;

         if(global_trans_b == 1)
         {    
            if(je_start_transaction(command_list_p,
                     CM_CMD_SET_PARAMS,
                     result_p,
                     mgmt_engine_id) != OF_SUCCESS)
            {
               je_free_command_list (command_list_p);
               CM_JE_DEBUG_PRINT("start transaction failed");
               return OF_FAILURE;
            }
         }

         /* Increment Command Count */
         config_session_p->pCache->no_of_commands++;

         /* Assign Sequuest ID to Command List */
         command_list_p->sequence_id = config_session_p->pCache->no_of_commands;

         /* Append New Command List to MOD DLLQ List */
         if(parent_trans_b == TRUE || global_trans_b == TRUE)
         {
            CM_DLLQ_APPEND_NODE (&(config_session_p->pCache->xtn_config_list),
                  &command_list_p->list_node);
            command_list_p->xtn_b = TRUE;
         }
         else
         {
            CM_DLLQ_APPEND_NODE (&(config_session_p->pCache->modify_config_list),
                  &(command_list_p->list_node));
         }

         CM_JE_DEBUG_PRINT ("Command Succesfully Added to Modify Cache");
         break;

      case CM_CMD_EXEC_TRANS_CMD:
         CM_JE_DEBUG_PRINT ("execute Command");
         CM_DLLQ_SCAN (&(config_session_p->pCache->xtn_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            if  ((!strcmp (command_list_p->command_p->dm_path_p, config_request_p->dm_path_p))
                  &&(command_list_p->exec_command_b != TRUE))
            {
               if(je_execute_done_command(config_session_p,
                        command_list_p,
                        command_list_p->command_p->command_id,
                        result_p) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT("command  execution failed");
                  return OF_FAILURE;
               }
               command_list_p->exec_command_b = TRUE;
               // return OF_SUCCESS;
            }    
         }
         break;

      case CM_CMD_CANCEL_TRANS_CMD:
         CM_JE_DEBUG_PRINT ("cancel Command");
         /* used to delete an existing command in transaction*/
         CM_DLLQ_SCAN (&(config_session_p->pCache->xtn_config_list), pDllQNode,
               UCMDllQNode_t *)
         {    
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            if (!strcmp (command_list_p->command_p->dm_path_p, config_request_p->dm_path_p))
            {
               CM_DLLQ_DELETE_NODE (&(config_session_p->pCache->xtn_config_list),
                     (UCMDllQNode_t *) command_list_p);
               config_session_p->pCache->no_of_commands--;
               je_free_command_list (command_list_p);
               return OF_SUCCESS;
            }
         }
         break; 
      case CM_CMD_DEL_PARAMS:
         CM_JE_DEBUG_PRINT ("Delete Command");
         CM_INC_STATS_JESESSION (JE_SESSION_DEL_PARAMS);
         CM_DLLQ_SCAN (&(config_session_p->pCache->add_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            /*if ADD  Command Followed by Delete 
             * Remove Both from List
             */
            if (!strcmp (command_list_p->command_p->dm_path_p, config_request_p->dm_path_p)
                  && command_list_p->exec_command_b != TRUE)
            {
               CM_DLLQ_DELETE_NODE (&(config_session_p->pCache->add_config_list),
                     (UCMDllQNode_t *) command_list_p);
               je_free_command_list (command_list_p);
               return OF_SUCCESS;
            }
         }

         CM_DLLQ_SCAN (&(config_session_p->pCache->delete_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            /* Check For Dublicate */
            if (!strcmp (command_list_p->command_p->dm_path_p, dm_path_p))
            {
               CM_JE_DEBUG_PRINT ("Duplicate Node found");
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
                  (result_p, UCMJE_ERROR_DMPATH_INVALID, dm_path_p);
               return OF_FAILURE;
            }
         }

         CM_DLLQ_SCAN (&(config_session_p->pCache->xtn_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            if (!strcmp (command_list_p->command_p->dm_path_p, dm_path_p) 
                  && command_list_p->exec_command_b != TRUE && 
                  command_list_p->command_p->command_id == CM_CMD_ADD_PARAMS)
            {
               CM_DLLQ_DELETE_NODE (&(config_session_p->pCache->xtn_config_list),
                     (UCMDllQNode_t *) command_list_p);
               je_free_command_list (command_list_p);
               return OF_SUCCESS;
            }
            else if (!strcmp (command_list_p->command_p->dm_path_p, dm_path_p)
                  && command_list_p->command_p->command_id == CM_CMD_DEL_PARAMS)
            {
               CM_JE_DEBUG_PRINT ("Duplicate Node found");
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
                  (result_p, UCMJE_ERROR_DMPATH_INVALID, dm_path_p);
               return OF_FAILURE;
            }
         }

         if(parent_trans_b != TRUE)
         {
#if CM_ROLES_PERM_SUPPORT
            if(pDMParentNode->element_attrib.non_config_leaf_node_b != FALSE &&
                  pDMParentNode->element_attrib.non_config_leaf_node_b != TRUE)
            {
               /* Roles and permissions will be verified after removing instance
                  when non config leaf node is TRUE_MOD_DEL or TRUE_DEL or TRUE_ADD*/
               parent_path_p = (char *)of_calloc(1,uiPathLen+1);
               cm_remove_last_node_instance_from_dmpath(dm_path_p,parent_path_p);
               dm_path_p = parent_path_p;
            }        

            if (cm_verify_role_permissions (dm_path_p, config_request_p->admin_role,
                     config_request_p->sub_command_id) != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Permission Denied");
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                     UCMJE_ERROR_PERMISSION_DENIED,
                     dm_path_p);
               if(parent_path_p != NULL)
               {
                  of_free(parent_path_p);
                  parent_path_p = NULL;
               }
               return OF_FAILURE;
            }

            if(parent_path_p != NULL)
            {
               of_free(parent_path_p);
               parent_path_p = NULL;
            }
            dm_path_p = config_request_p->dm_path_p;
#endif
         }

         /* Allocate Memory for New Command List */
         command_list_p = of_calloc (1, sizeof (struct je_command_list));
         if (!command_list_p)
         {
            CM_JE_DEBUG_PRINT ("Memory Allocation for Command List Failed");
            return OF_FAILURE;
         }

         /* Add request to New Command List  */
         if (je_cfgsess_add_to_new_commandlist (command_list_p, config_request_p) !=
               OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Add to New Command List Failed");
            of_free (command_list_p);
            return OF_FAILURE;
         }

         command_list_p->command_p->command_id = CM_CMD_DEL_PARAMS;
         if(global_trans_b == 1)
         {
            if(je_start_transaction(command_list_p,
                     CM_CMD_DEL_PARAMS,
                     result_p,
                     mgmt_engine_id) != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT("start transaction failed");
               config_session_p->pCache->no_of_commands--;
               je_free_command_list (command_list_p);
               return OF_FAILURE;
            }
         }

         /* Increment Command Count */
         config_session_p->pCache->no_of_commands++;

         /* Assign Sequuest ID to Command List */
         command_list_p->sequence_id = config_session_p->pCache->no_of_commands;


         if(parent_trans_b == TRUE || global_trans_b == TRUE)
         {
            CM_DLLQ_APPEND_NODE (&(config_session_p->pCache->xtn_config_list),
                  &(command_list_p->list_node));
            command_list_p->xtn_b = TRUE;
         }
         else
         {
            /* Append New Command List to DEL DLLQ List */
            CM_DLLQ_APPEND_NODE (&(config_session_p->pCache->delete_config_list),
                  &(command_list_p->list_node));
         }


         CM_JE_DEBUG_PRINT ("Command Succesfully Added to Delete Cache");
         break;

      case CM_CMD_CANCEL_PREV_CMD:
         CM_JE_DEBUG_PRINT ("Cancel Previous Command");
         CM_INC_STATS_JESESSION (JE_SESSION_CANCEL_PREV_CMD);
         dm_path_p = config_request_p->dm_path_p;
         pCancelCmdName = config_request_p->data.nv_pair_array.nv_pairs[0].name_p;

         pDMParentNode =
            (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path (dm_path_p,
                  of_strlen
                  (dm_path_p));
         if (!pDMParentNode)
         {
            CM_JE_DEBUG_PRINT ("pDParentNode is NULL");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_GETNODE_BYDMPATH,
                  dm_path_p);
            return OF_FAILURE;
         }

         pDMNode = (struct cm_dm_data_element *) cmi_dm_get_child_by_name (pDMParentNode,
               pCancelCmdName);
         if (pDMNode == NULL)
         {
            CM_JE_DEBUG_PRINT ("GetChildByName %s for Parent %s Failed",
                  pCancelCmdName, pDMParentNode->name_p);
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_GETCHILD_BYNAME,
                  dm_path_p);
            return OF_FAILURE;
         }

         if (pDMNode->element_attrib.key_b == TRUE)
         {
            CM_JE_DEBUG_PRINT ("Key cannot be deleted");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_KEY_CANNOT_BE_CANCELED,
                  dm_path_p);
            return OF_FAILURE;
         }

         CM_DLLQ_SCAN (&(config_session_p->pCache->add_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            if (!of_strcmp (command_list_p->command_p->dm_path_p, config_request_p->dm_path_p))
            {
               for (uiNvCnt = 0;
                     uiNvCnt < command_list_p->command_p->nv_pair_array.count_ui; uiNvCnt++)
               {
                  if (!of_strcmp
                        (pCancelCmdName,
                         command_list_p->command_p->nv_pair_array.nv_pairs[uiNvCnt].name_p))
                  {
                     CM_JE_DEBUG_PRINT ("NvPair Found in Add Cache of Session");
                     /* Allocate Memory for New Command */
                     if (return_value =
                           je_delete_nvpair_from_commandlist (command_list_p,
                              pCancelCmdName,
                              result_p) != OF_SUCCESS)
                     {
                        CM_JE_DEBUG_PRINT ("Delete Nv Pair from CommandList failed");
                        return OF_FAILURE;
                     }
                     return OF_SUCCESS;
                  }
               }
            }
         }

         CM_DLLQ_SCAN (&(config_session_p->pCache->modify_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            if (!of_strcmp (command_list_p->command_p->dm_path_p, config_request_p->dm_path_p))
            {
               for (uiNvCnt = 0;
                     uiNvCnt < command_list_p->command_p->nv_pair_array.count_ui; uiNvCnt++)
               {
                  if (!of_strcmp
                        (pCancelCmdName,
                         command_list_p->command_p->nv_pair_array.nv_pairs[uiNvCnt].name_p))
                  {
                     CM_JE_DEBUG_PRINT ("NvPair Found in Add Cache of Session");
                     /* Allocate Memory for New Command */
                     if (return_value =
                           je_delete_nvpair_from_commandlist (command_list_p,
                              pCancelCmdName,
                              result_p) != OF_SUCCESS)
                     {
                        CM_JE_DEBUG_PRINT ("Delete Nv Pair from CommandList failed");
                        return OF_FAILURE;
                     }
                     return OF_SUCCESS;
                  }
               }
            }
         }

         CM_DLLQ_SCAN (&(config_session_p->pCache->delete_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            if (!of_strcmp (command_list_p->command_p->dm_path_p, config_request_p->dm_path_p))
            {
               for (uiNvCnt = 0;
                     uiNvCnt < command_list_p->command_p->nv_pair_array.count_ui; uiNvCnt++)
               {
                  if (!of_strcmp
                        (pCancelCmdName,
                         command_list_p->command_p->nv_pair_array.nv_pairs[uiNvCnt].name_p))
                  {
                     CM_JE_DEBUG_PRINT ("NvPair Found in Add Cache of Session");
                     /* Allocate Memory for New Command */
                     if (return_value =
                           je_delete_nvpair_from_commandlist (command_list_p,
                              pCancelCmdName,
                              result_p) != OF_SUCCESS)
                     {
                        CM_JE_DEBUG_PRINT ("Delete Nv Pair from CommandList failed");
                        return OF_FAILURE;
                     }
                     return OF_SUCCESS;
                  }
               }
            }
         }

         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
               UCMJE_ERROR_CANCEL_COMMAND_NOT_FOUND,
               dm_path_p);
         return OF_FAILURE;

      case CM_CMD_SET_PARAM_TO_DEFAULT:
         CM_JE_DEBUG_PRINT ("Set to Default Command");
         CM_INC_STATS_JESESSION (JE_SESSION_SET_PARAM_TO_DEFAULT);
         if (je_set_nvpair_array_to_default_values
               (config_request_p->dm_path_p, &(config_request_p->data.nv_pair_array), result_p)
               != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Set Default Values Failed");
            return OF_FAILURE;
         }

         /* Verify Command Already present in  ADD Cache?? */
         CM_DLLQ_SCAN (&(config_session_p->pCache->add_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            if (!strcmp (command_list_p->command_p->dm_path_p, config_request_p->dm_path_p))
            {
               je_cfgsess_add_to_exist_commandlist (command_list_p, config_request_p);
            }
         }

         /* Verify Command Already present in  MODIFY Cache?? */
         CM_DLLQ_SCAN (&(config_session_p->pCache->modify_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            /* Replace Exisiting Name Value Pairs */
            if (!strcmp (command_list_p->command_p->dm_path_p, config_request_p->dm_path_p))
            {
               je_cfgsess_add_to_exist_commandlist (command_list_p, config_request_p);
            }
         }
         break;

      default:
         CM_JE_DEBUG_PRINT ("Unknown command");
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
            (result_p, UCMJE_ERROR_UNKNOWN_SUB_COMMAND, dm_path_p);
         return OF_FAILURE;
   }
   return OF_SUCCESS;
}

/**
  \ingroup JEAP

  This API Validates, Verifies Commands in Configuration Cache and invokes security
  applications registered routines.

  if the operation is commit, it does the UCM basic validations using Data Model
  information.It Executes application Test functions on all of the commands in 
  Configuration Session.  If all tests are successful, Execute the application callback
  functions.
  If all callback functions are successful, delete configuration session, send the
  response on transport channel.
  If any test functions or registered security application routines frames error messages
  and return it to transport channel. 
  <b>Input paramameters: </b>\n
  <b><i> config_session_p: </i></b> Pointer to Configuration Session for which request
  belongs to 
  <b><i> config_request_p: </i></b> pointer to Configuration Request sent by Management
  Engines.

  <b>Output Parameters: </b>
  <b><i>pUCMResult:</i></b> pointer to UCMResult 
  In Success case it contains data for Get Requests
  In Failure case error information \n

  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_sm_cfgsess_commit_handler
 ** Description   : This API Validates, Verifies Commands in Configuration Cache
 **                 and invokes security applications registered routines.
 ** Input params  : config_session_p - Pointer to Configuration Session
 **                 config_request_p - pointer to Configuration Request sent by Management
 **                              Engines.
 ** Output params : result_p - pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t je_sm_cfgsess_commit_handler (struct je_config_session *
      config_session_p,
      struct je_config_request *
      config_request_p,
      struct cm_result * result_p)
{
   uint32_t return_value = OF_FAILURE;

   struct je_command_list *command_list_p = NULL, *pTmp;
   uint16_t usCbkSuccessCnt = 0;
   int32_t iCbkRet;

#ifdef CM_VERSION_SUPPORT
   uint64_t NewCfgVer = 0;
#ifdef  CM_EVENT_NOTIFMGR_SUPPORT
   struct cm_evmgr_event Event;
   time_t t;
#endif
#endif

   CM_JE_DEBUG_PRINT ("Entered");
   if (config_session_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("config_session_p is NULL");
      return OF_FAILURE;
   }
   if (config_session_p->pCache == NULL)
   {
      CM_JE_DEBUG_PRINT ("Configuration Cache is not initialized");
      return OF_FAILURE;
   }
   if (config_request_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("command_p is NULL");
      return OF_FAILURE;
   }

   if (return_value = je_verify_command_referrence (config_session_p, result_p) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Command Verification failed");
      return OF_FAILURE;
   }

   if (return_value = je_check_session_mandatory_params (config_session_p, result_p) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Mandatory Parameter is missing");
      return OF_FAILURE;
   }

   if (return_value = je_execute_test_func_handler (config_session_p, result_p) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Test Func failed");
      return OF_FAILURE;
   }

   if (iCbkRet =
         je_execute_cbk_func_handler (config_session_p, &usCbkSuccessCnt,
            result_p) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Execute Cbk  failed");
      if (usCbkSuccessCnt == 0)
      {
         return OF_FAILURE;
      }
   }

#ifdef CM_AUDIT_SUPPORT
   /* prepare UCMAuditData */
   if ((return_value = cm_generate_audit_log_message (config_session_p, result_p)) != OF_SUCCESS)
   {
      return UCMJE_ERROR_GENERATE_AUDITLOG_FAILED;
   } 

#endif
#ifdef CM_VERSION_SUPPORT
   if ((usCbkSuccessCnt > 0)
         && (config_session_p->mgmt_engine_id != CM_LDSV_MGMT_ENGINE_ID))
   {
      if ((return_value =
               cm_je_add_command_cache_to_config_version (config_session_p, &NewCfgVer,
                  result_p)) != OF_SUCCESS)
      {
         return UCMJE_ERROR_GET_NEWCONFIGVERSION_FAILED;
      }
   }
#ifdef CM_EVENT_NOTIFMGR_SUPPORT

   /*
    * Presently notification is sent for every New version.
    * For initial Load configuration and Factory reset may
    * require separate notification. It may be required to
    * consolidate and send only one notification
    * to the intrested modules in such cases.
    */

   if ((NewCfgVer > 0) &&
         ( config_session_p->mgmt_engine_id != CM_LDSV_MGMT_ENGINE_ID))
   {
      CM_JE_DEBUG_PRINT (" ****NewCfgVer = %llx", NewCfgVer);

      Event.mgmt_engine_id = config_session_p->mgmt_engine_id;
      Event.date_time = time (&t);
      Event.event_type = CM_CONFIG_VERSION_UPDATE_EVENT;
      Event.Event.version_change.version = NewCfgVer;

      if ((return_value = cm_evmgr_generate_event (&Event)) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Failed to Notify Event.");
         result_p->result.error.error_code =
            UCMJE_ERROR_NOTIFICATION_CONFIGUPDATE_FAILED;
         //return OF_FAILURE;
      }
   }
#endif /* CM_EVENT_NOTIFMGR_SUPPORT */

#endif /* CM_VERSION_SUPPORT */

   /* De Allocating All Commands from Cache
    * De Allocating  Cache also */

   if (iCbkRet == OF_SUCCESS)
   {
      cmje_cfgsess_clean_cache (config_session_p->pCache);
      cm_je_cfgsession_free (config_session_p);
      CM_INC_STATS_JE (JE_CFGSESSION_CLOSED);
      CM_INC_STATS_JE (JE_CFGSESSION_COMMITTED);
   }
   else
   {
      CM_DLLQ_DYN_SCAN (&(config_session_p->pCache->add_config_list), command_list_p, pTmp,
            struct je_command_list *)
      {
         /* Delete only Successfully Executed Commands */
         if ((command_list_p) && (command_list_p->success_b == TRUE))
         {
            CM_DLLQ_DELETE_NODE (&(config_session_p->pCache->add_config_list),
                  (UCMDllQNode_t *) command_list_p);
            je_free_command_list (command_list_p);
            config_session_p->pCache->no_of_commands--;
         }
      }

      CM_DLLQ_DYN_SCAN (&(config_session_p->pCache->modify_config_list), command_list_p, pTmp,
            struct je_command_list *)
      {
         if ((command_list_p) && (command_list_p->success_b == TRUE))
         {
            CM_DLLQ_DELETE_NODE (&(config_session_p->pCache->modify_config_list),
                  (UCMDllQNode_t *) command_list_p);
            je_free_command_list (command_list_p);
            config_session_p->pCache->no_of_commands--;
         }
      }

      CM_DLLQ_DYN_SCAN (&(config_session_p->pCache->delete_config_list), command_list_p, pTmp,
            struct je_command_list *)
      {
         if ((command_list_p) && (command_list_p->success_b == TRUE))
         {
            CM_DLLQ_DELETE_NODE (&(config_session_p->pCache->delete_config_list),
                  (UCMDllQNode_t *) command_list_p);
            je_free_command_list (command_list_p);
            config_session_p->pCache->no_of_commands--;
         }
      }

      CM_DLLQ_DYN_SCAN (&(config_session_p->pCache->xtn_config_list), command_list_p, pTmp,
            struct je_command_list *)
      {
         if((command_list_p) && (command_list_p->state == COMMAND_EXECUTED))
         {
            CM_DLLQ_DELETE_NODE (&(config_session_p->pCache->xtn_config_list),
                  (UCMDllQNode_t *) command_list_p);
            je_free_command_list (command_list_p);
            config_session_p->pCache->no_of_commands--;
         }
      }
   }
   return iCbkRet;
}


/**
  \ingroup JEAPI

  This Executes a single command and does audit message but not config versioning.

  <b>Input paramameters: </b>\n
  <b><i> config_session_p: </i></b> Pointer to Configuration Session for which request
  belongs to 
  <b><i> config_request_p: </i></b> pointer to Configuration Request sent by Management
  Engines.

  <b>Output Parameters: </b>
  <b><i>pUCMResult:</i></b> pointer to UCMResult 
  In Success case it contains data for Get Requests
  In Failure case error information \n

  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_execute_command
 ** Description   : This API executes a single command inside a transaction
 ** Input params  : config_session_p - Pointer to Configuration Session
 **                 config_request_p - pointer to Configuration Request sent by Management
 **                              Engines.
 ** Output params : result_p - pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t je_execute_command( struct je_config_session *config_session_p,
      struct je_command_list *pInCommandList,
      uint32_t command_id,
      struct cm_result *result_p)
{
   UCMDllQ_t *pDll=NULL;
   struct cm_dm_data_element *pDMNode = NULL;
   struct cm_dm_data_element *pChildNode = NULL;
   UCMDllQNode_t *pNode=NULL;
   UCMDllQNode_t *pDllQNode=NULL;
   struct cm_array_of_nv_pair key_nv_pairs={};
   uint32_t uiNvCnt=0;
   uint32_t ureturn_value=0,uiJeCommand=0;
   uint32_t uiPathLen = 0;
   uint32_t mgmt_engine_id = 0;
   char *dm_path_p = NULL;
   unsigned char bAddInst= FALSE;
   unsigned char referrence_in_cache_b;
   char *parent_path_p = NULL;
   struct je_command_list  *command_list_p=NULL;
   struct cm_array_of_nv_pair *pNVpairs=NULL;
   struct cm_array_of_iv_pairs IvPairArray = { };
   struct cm_array_of_nv_pair *dmpath_key_nv_pairs_p = NULL;
   struct cm_array_of_nv_pair *result_nv_pairs_p = NULL;


   mgmt_engine_id = config_session_p->mgmt_engine_id;
   dm_path_p =  pInCommandList->command_p->dm_path_p;
   pNVpairs = &pInCommandList->command_p->nv_pair_array;
   referrence_in_cache_b = pInCommandList->referrence_in_cache_b;

   if (!dm_path_p || of_strlen(dm_path_p) == 0)
   {
      CM_JE_DEBUG_PRINT ("DM Path is NULL");
      result_p->result.error.error_code = UCMJE_ERROR_DMPATH_NULL;
      return OF_FAILURE;
   }


   uiPathLen = of_strlen(dm_path_p);
   parent_path_p = (char *)of_calloc(1,uiPathLen+1);
   cm_remove_last_node_instance_from_dmpath(dm_path_p,parent_path_p);

   pDMNode = (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path
      (dm_path_p, uiPathLen);
   if (!pDMNode)
   {
      CM_JE_DEBUG_PRINT ("pDMNode is NULL");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_GETNODE_BYDMPATH,
            dm_path_p);
      of_free(parent_path_p);
      return OF_FAILURE;
   }

   if(pDMNode->element_attrib.parent_trans_b != 1)
   {
      CM_JE_DEBUG_PRINT("Not a command");
      of_free(parent_path_p);
      return OF_FAILURE;
   }

   /*Check mandatory parameters*/
   /*use utility API to check mandatory parameters ...????*/
   /*This will be checked only for the addition of records (ie for the first time)*/
   if(command_id == CM_COMMAND_CONFIG_SESSION_START)
   {
      pDll = &(pDMNode->child_list);
      CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
      {
         pChildNode =
            CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
         if (pChildNode->element_attrib.mandatory_b == TRUE)
         {
            struct cm_array_of_nv_pair *pNvPairAry = pNVpairs;
            unsigned char bNameFound = FALSE;

            for (uiNvCnt = 0; uiNvCnt < pNvPairAry->count_ui; uiNvCnt++)
            {
               if (!of_strcmp(pChildNode->name_p, pNvPairAry->nv_pairs[uiNvCnt].name_p))
               {
                  bNameFound = TRUE;
               }
            }
            if(bNameFound == FALSE)
            {
               if(pChildNode->element_attrib.visible_b == FALSE)
               {
                  CM_JE_DEBUG_PRINT ("%s not a configurable parameter",
                        pChildNode->name_p);
                  continue;
               }

               CM_JE_DEBUG_PRINT ("%s not set", pChildNode->name_p);
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                     UCMJE_ERROR_MANDATORY_PARAM_NOT_SET,
                     dm_path_p);
               UCMJE_MEMALLOC_AND_COPY_RESULTSTRING_INTO_UCMRESULT (result_p,
                     pChildNode->name_p);
               of_free(parent_path_p);
               return OF_FAILURE;
            }
         }
      }
   }
   /***Executing Test Function****/

   if(je_execute_command_test_func(parent_path_p,
            pNVpairs,
            command_id,
            referrence_in_cache_b,
            result_p,
            mgmt_engine_id)!=OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT("Test function failed");
      of_free(parent_path_p);
      return OF_FAILURE;
   }

   /***Executing Callback Function***/

   if(command_id == CM_CMD_SET_PARAMS || command_id == CM_CMD_ADD_PARAMS)
   {
      if((ureturn_value = je_execute_command_cbk_func (parent_path_p,
                  pNVpairs,
                  command_id,
                  result_p,
                  EXEC_COMPLETE_CMD,
                  mgmt_engine_id)) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Execute Command Cbk Failed");
         of_free(parent_path_p);
         return OF_FAILURE;
      }
   }
   if(command_id == CM_CMD_DEL_PARAMS)
   {
      /*call del function directly do not delete instances */
      /*Instances will be deleted after commit transaction is success */

      if ((ureturn_value = cmi_dm_get_key_array_from_namepath (parent_path_p,
                  &dmpath_key_nv_pairs_p))!= OF_SUCCESS)
      {
         if(ureturn_value == OF_FAILURE)
         {
            ureturn_value = UCMJE_ERROR_DMGET_KEYS_FROM_NAMEPATH_FAILED;
         }         
         CM_JE_DEBUG_PRINT ("Get Keys Array From Name Path failed");
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
               ureturn_value, dm_path_p);
         of_free(parent_path_p);
         return OF_FAILURE;
      }

      if ((ureturn_value = je_combine_nvpairs (NULL,
                  dmpath_key_nv_pairs_p,
                  pNVpairs,
                  &result_nv_pairs_p)) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Combining Nv Pair Arrays failed");
         cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
         of_free(parent_path_p);
         return OF_FAILURE;
      }

      /* Convert Input Nv Pair Array to IV Pair Array */
      if ((je_copy_nvpairarr_to_ivpairarr (dm_path_p,
                  result_nv_pairs_p,
                  &IvPairArray,
                  mgmt_engine_id)) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Convertion to IV Pairs Failed");
         cm_je_free_ivpair_array (&IvPairArray);
         cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
         cm_je_free_ptr_nvpairArray (result_nv_pairs_p);
         of_free(parent_path_p);
         return OF_FAILURE;
      }


      if(cmje_execute_delete_command(pDMNode, &IvPairArray, result_p,EXEC_COMPLETE_CMD) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT("Command to delete record failed");
         cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
         cm_je_free_ptr_nvpairArray (result_nv_pairs_p);
         of_free(parent_path_p);
         return OF_FAILURE;
      }
   }

   pInCommandList->exec_command_b = TRUE;
   pInCommandList->state = COMMAND_EXECUTED;
   cm_je_free_ivpair_array (&IvPairArray);
   cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
   cm_je_free_ptr_nvpairArray (result_nv_pairs_p);


   if(command_id == CM_CMD_DEL_PARAMS)
   {
      pInCommandList->del_instance_tree_b = FALSE;
      CM_DLLQ_SCAN (&(config_session_p->pCache->xtn_config_list), pDllQNode,
            UCMDllQNode_t *)
      {
         command_list_p =
            CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
         if (!strcmp (command_list_p->command_p->dm_path_p, dm_path_p)
               && command_list_p->command_p->command_id == CM_CMD_ADD_PARAMS)
         {
            pInCommandList->del_instance_tree_b = command_list_p->del_instance_tree_b;
            CM_DLLQ_DELETE_NODE (&(config_session_p->pCache->add_config_list),
                  (UCMDllQNode_t *) command_list_p);
            /* Delete Node from Transaction Configuraton List also????? */
            je_free_command_list (command_list_p);
         }
      }
   }

   if(command_id == CM_CMD_ADD_PARAMS)
   {
      pInCommandList->del_instance_tree_b = TRUE;
      CM_DLLQ_SCAN (&(config_session_p->pCache->xtn_config_list), pDllQNode,
            UCMDllQNode_t *)
      {
         command_list_p =
            CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
         if (!strcmp (command_list_p->command_p->dm_path_p, dm_path_p)
               && command_list_p->command_p->command_id == CM_CMD_DEL_PARAMS)
         {
            bAddInst = FALSE;
            pInCommandList->del_instance_tree_b = command_list_p->del_instance_tree_b;
            CM_DLLQ_DELETE_NODE (&(config_session_p->pCache->add_config_list),
                  (UCMDllQNode_t *) command_list_p);
            /* Delete Node from Transaction Configuraton List also????? */
            je_free_command_list (command_list_p);
         }
      }
   }

   if((ureturn_value = je_get_key_nvpair_arr (dm_path_p, pNVpairs,
               &key_nv_pairs)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Get Key NvPair Array Failed");
      of_free(parent_path_p);
      return OF_FAILURE;
   }
   /*
      if(command_id == CM_CMD_ADD_PARAMS)
      {
      if((ureturn_value = je_add_instance_map_entry (parent_path_p, &key_nv_pairs)) != OF_SUCCESS)
      {
   //one flag needs to be set to Instance to tell that its a temp instance ...

   CM_JE_DEBUG_PRINT ("JE Add Instance Map Entry  failure");
   UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
   ureturn_value, dm_path_p);
   je_free_nvpair_array (&key_nv_pairs);
   of_free(parent_path_p);
   return OF_FAILURE;
   }

#ifdef CM_ROLES_PERM_SUPPORT
if((ureturn_value = cm_je_set_role_to_instance (dm_path_p,
config_session_p->admin_role,
&key_nv_pairs)) != OF_SUCCESS)
{
CM_JE_DEBUG_PRINT ("JE Set Role Permissions  failure");
UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
ureturn_value, dm_path_p);
je_free_nvpair_array (&key_nv_pairs);
of_free(parent_path_p);
return OF_FAILURE;
}
#endif
}
*/

/*** Auditing the records ***/
if(command_id == CM_CMD_ADD_PARAMS || command_id == CM_CMD_SET_PARAMS)
{
   switch(command_id)
   {
      case CM_CMD_ADD_PARAMS: uiJeCommand = JE_MSG_CONFIG_ADD;
                               break;
      case CM_CMD_SET_PARAMS: uiJeCommand = JE_MSG_CONFIG_SET;
                               break;
                               /*    default:
                                     CM_JE_DEBUG_PRINT("Unhandled subcommand"); CID - 55*/
   }
   /* This API  needs to be updated to accept multiple keys ...???*/ 
#ifdef CM_AUDIT_SUPPORT
   UCMJEFrameAndSendLogMsg (uiJeCommand,
         config_session_p->admin_name,
         config_session_p->admin_role,
         command_id, parent_path_p,
         key_nv_pairs.nv_pairs[0].value_p,
         pNVpairs);
#endif   
   je_free_nvpair_array (&key_nv_pairs);
}

of_free(parent_path_p);
return OF_SUCCESS;

}


/******************************************************************************
 ** Function Name : ucmJEStarttransaction
 ** Description   : This API executes start transaction for applications 
 **                                        supporting global transaction.
 ** Input params  : 
 ** Output params :  
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/

 int32_t je_start_transaction( struct je_command_list *pInCommandList,
      uint32_t command_id,
      struct cm_result *result_p, uint32_t mgmt_engine_id)
{
   unsigned char bFound = 0;
   UCMDllQNode_t *pNode=NULL;
   UCMDllQ_t *pDll=NULL;
   struct je_xtn_entry *xtn_entry_p = NULL;
   struct cm_dm_data_element *pDMNode = NULL;
   struct cm_dm_data_element *pChildNode = NULL;
   uint32_t appl_id_ui, channel_id_ui, uiNvCnt;
   int32_t return_value = 0,icRet = 0;
   cm_channel_start_xtn_cbk_p pBeginFn;
   cm_channel_end_xtn_cbk_p pEndFn;
   struct cm_array_of_nv_pair *dmpath_key_nv_pairs_p = NULL;
   struct cm_array_of_iv_pairs mand_iv_pairs = { };
   struct cm_array_of_iv_pairs opt_iv_pairs = { };
   struct cm_array_of_nv_pair key_nv_pairs={};
   struct cm_array_of_nv_pair *pNVpairs=NULL;
   char *dm_path_p=NULL;
   unsigned char referrence_in_cache_b;
   struct cm_array_of_iv_pairs  KeyIvPairArr = { };
   void *xtn_rec = NULL;
   struct cm_app_result *app_result_p;
   char *parent_path_p=NULL;
   uint32_t uiPathLen;
   unsigned char global_trans_b=FALSE;
   unsigned char parent_trans_b=FALSE;

   dm_path_p =  pInCommandList->command_p->dm_path_p;
   pNVpairs = &pInCommandList->command_p->nv_pair_array;
   referrence_in_cache_b = pInCommandList->referrence_in_cache_b;


   /**** Verify transaction record already exists for a given dmpath ****/

   CM_DLLQ_SCAN(&(UCMJEGloTransaction_g), xtn_entry_p, struct je_xtn_entry*)
   {
      if(of_strcmp(dm_path_p,xtn_entry_p->pcDMPath)== 0)
      {
         bFound = 1;
         if(xtn_entry_p->status_b  == 0)
         {
            /***Transaction record exists but Transaction need to be started***/
            break;
         }
         CM_JE_DEBUG_PRINT("Transaction already started");
         return OF_SUCCESS;
      }
   }

   pDMNode = (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path
      (dm_path_p, of_strlen (dm_path_p));
   if (!pDMNode)
   {
      CM_JE_DEBUG_PRINT ("pDMNode is NULL");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_GETNODE_BYDMPATH,
            dm_path_p);
      return OF_FAILURE;
   }

   if(pDMNode->element_attrib.global_trans_b == TRUE)
   {
      global_trans_b = TRUE;
   }

   if(pDMNode->element_attrib.parent_trans_b == TRUE)
   {
      parent_trans_b = TRUE;
   }

   /****** Look for key parameters before starting transaction *********/

   /* CALL common utility API :je_get_key_nvpair_arr ...???*/
   pDll = &(pDMNode->child_list);
   CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
   {
      pChildNode =
         CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
      if (pChildNode->element_attrib.key_b == TRUE)
      {
         struct cm_array_of_nv_pair *pNvPairAry = pNVpairs;
         unsigned char bNameFound = FALSE;

         for (uiNvCnt = 0; uiNvCnt < pNvPairAry->count_ui; uiNvCnt++)
         {
            if (!of_strcmp(pChildNode->name_p, pNvPairAry->nv_pairs[uiNvCnt].name_p))
            {
               bNameFound = TRUE;
            }
         }
         if(bNameFound == FALSE)
         {
            CM_JE_DEBUG_PRINT ("%s not set", pChildNode->name_p);
            CM_JE_DEBUG_PRINT("key parameter not yet given,not starting transaction");
            return OF_SUCCESS;
         }
      }
   }
   /**** Fix Me *****/
   /*** What to do if transaction requires mandatory parameters also ***/

   /****Get Channel ID and Application Id to get function pointer for start transaction***/

   icRet = cm_get_channel_id_and_appl_id (dm_path_p, &appl_id_ui, &channel_id_ui);
   if (icRet == OF_FAILURE)
   {
      CM_JE_DEBUG_PRINT("Get channel ID and Application ID failed");
      return OF_FAILURE;
   }
   if (!(pDMNode->app_cbks_p))
   {
      if (cm_channels_p[channel_id_ui] == NULL)
      {
         CM_JE_DEBUG_PRINT ("Channel Callbacks are NULL");
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
               UCMJE_ERROR_APPCBK_NULL,
               dm_path_p);
         return OF_FAILURE;
      }
   }

   pBeginFn = cm_channels_p[channel_id_ui]->cm_start_xtn;

   pEndFn = cm_channels_p[channel_id_ui]->cm_end_xtn;

   /*** Make sure applications has both begin and end transaction ***/
   if (!pBeginFn || !pEndFn && global_trans_b == TRUE)
   {
      CM_JE_DEBUG_PRINT ("Begin and End transactions are required for application supporting global transaction");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_INVALID_PAIR_BEGINENDCBKS,
            dm_path_p);
      return OF_FAILURE;
   }

   if(!pBeginFn || !pEndFn)
   {
      pInCommandList->state = EXEC_CBK_FUNC;
      return OF_SUCCESS;
   }

   if(parent_trans_b == TRUE)
   {
      if((return_value = je_get_trans_rec(dm_path_p,&xtn_rec)) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT("Failed to get transaction Record");
         return OF_FAILURE;
      }

   }
   else if(bFound == 0)
   {
      xtn_entry_p = of_calloc (1, sizeof (struct je_xtn_entry));
      if (!xtn_entry_p)
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation for Transaction Failed");
         return OF_FAILURE;
      }
      CM_DLLQ_INIT_LIST(&xtn_entry_p->CmdList);
      CM_DLLQ_APPEND_NODE(&(UCMJEGloTransaction_g),&xtn_entry_p->list_node);
      xtn_entry_p->pcDMPath = (char*)of_calloc(1, of_strlen(dm_path_p)+1);
      if(xtn_entry_p->pcDMPath  == NULL)
      {
         CM_JE_DEBUG_PRINT ("Allocating memory for DM Path failed");
         return OF_FAILURE;
      }
      of_strcpy(xtn_entry_p->pcDMPath,dm_path_p);
   }

   if ((return_value = cmi_dm_get_key_array_from_namepath (dm_path_p,
               &dmpath_key_nv_pairs_p))!= OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Get Keys Array From Name Path failed");
      CM_JE_DEBUG_PRINT ("Error:%d",return_value);
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            return_value,
            dm_path_p);
      return OF_FAILURE;
   }

   if ((return_value = je_make_ivpairs (dm_path_p, NULL,
               dmpath_key_nv_pairs_p, pNVpairs,
               &mand_iv_pairs, &opt_iv_pairs,
               &KeyIvPairArr, result_p,mgmt_engine_id)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Convertion to IV Pairs Failed");
      cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
      return OF_FAILURE;
   }
   /*** Execute start transaction function ***/
   if (pBeginFn)
   {
      xtn_rec = pBeginFn (appl_id_ui, &mand_iv_pairs, command_id,&app_result_p);
      if (!xtn_rec)
      {
         CM_JE_DEBUG_PRINT ("Begin Function returned failure");
         ucmJECopyApresult_pToUCMResult (dm_path_p, app_result_p, result_p);
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
               UCMJE_ERROR_BEGINCBK_FAILED,
               dm_path_p);
         cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
         cm_je_free_ivpair_array (&mand_iv_pairs);
         cm_je_free_ivpair_array (&opt_iv_pairs);
         cm_je_free_ivpair_array (&KeyIvPairArr);
         return OF_FAILURE;
      }
   }

   if(parent_trans_b == TRUE)
   {
      pInCommandList->state = EXEC_CBK_FUNC;
      return OF_SUCCESS;
   }

   xtn_entry_p->xtn_rec = xtn_rec;
   cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
   cm_je_free_ivpair_array (&mand_iv_pairs);
   cm_je_free_ivpair_array (&opt_iv_pairs);
   cm_je_free_ivpair_array (&KeyIvPairArr);

   /*** create a instance (special case) **/
   /***need to be deleted if end transaction fails ***/

   if((return_value = je_get_key_nvpair_arr (dm_path_p, pNVpairs,
               &key_nv_pairs)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Get Key NvPair Array Failed");
      return OF_FAILURE;
   }

   if(command_id == CM_CMD_ADD_PARAMS)
   {
      uiPathLen = of_strlen(dm_path_p);
      parent_path_p = (char *)of_calloc(1,uiPathLen+1);
      cm_remove_last_node_instance_from_dmpath(dm_path_p,parent_path_p);
      if((return_value = je_add_instance_map_entry (parent_path_p, &key_nv_pairs)) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("JE Add Instance Map Entry  failure");
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
               return_value, dm_path_p);
         je_free_nvpair_array (&key_nv_pairs);
         of_free(parent_path_p); /*CID 347*/
         return OF_FAILURE;
      }
      pInCommandList->del_instance_tree_b = TRUE;
   }
   je_free_nvpair_array (&key_nv_pairs);

   pInCommandList->state = EXEC_CBK_FUNC;
   xtn_entry_p->status_b  = 1;
   return OF_SUCCESS;
}


 int32_t DelInstances(struct je_config_session *config_session_p,struct cm_result * result_p)
{
   struct je_command_list *command_list_p=NULL;
   struct je_xtn_entry *xtn_entry_p=NULL; 
   UCMDllQNode_t *pDllQNode = NULL;
   struct cm_dm_data_element *pDMNode=NULL;
   cm_channel_end_xtn_cbk_p pEndFn;
   struct cm_app_result *app_result_p;
   int32_t return_value,icRet;
   uint32_t appl_id_ui, channel_id_ui;
   uint32_t uiPathLen;
   uint32_t mgmt_engine_id=0;
   unsigned char global_trans_b = FALSE;
   char *parent_path_p = NULL;

   mgmt_engine_id = config_session_p->mgmt_engine_id;

   CM_DLLQ_SCAN (&(config_session_p->pCache->xtn_config_list), pDllQNode, UCMDllQNode_t *)
   {
      global_trans_b = FALSE;
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      if (command_list_p && command_list_p->command_p 
            && command_list_p->command_p->command_id == CM_CMD_ADD_PARAMS)
      {
         char *dm_path_p = command_list_p->command_p->dm_path_p;

         if (!dm_path_p || of_strlen(dm_path_p) == 0)
         {
            CM_JE_DEBUG_PRINT ("DM Path is NULL");
            result_p->result.error.error_code = UCMJE_ERROR_DMPATH_NULL;
            return OF_FAILURE;
         }

         pDMNode = (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path
            (dm_path_p, of_strlen (dm_path_p));
         if (!pDMNode)
         {
            CM_JE_DEBUG_PRINT ("pDMNode is NULL");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_GETNODE_BYDMPATH,
                  dm_path_p);
            return OF_FAILURE;
         }

         if(pDMNode->element_attrib.global_trans_b == TRUE)
         {
            global_trans_b = 1;
         }

         uiPathLen = of_strlen(command_list_p->command_p->dm_path_p);
         parent_path_p = (char *)of_calloc(1,uiPathLen+1);

         cm_remove_last_node_instance_from_dmpath(command_list_p->command_p->dm_path_p, parent_path_p);

         if(global_trans_b == TRUE && command_list_p->del_instance_tree_b == TRUE)
         {
            if (return_value = cmje_delete_subtree(parent_path_p,
                     &(command_list_p->command_p->nv_pair_array),
                     result_p,DEL_INSTANCE_ONLY,EXEC_COMPLETE_CMD,
                     mgmt_engine_id) != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Delete Subtree failed");
               of_free(parent_path_p); //CID 344
               return OF_FAILURE;
            }
         }
         /*    else if(pDMNode->element_attrib.parent_trans_b == 1)
               {
               if (return_value = cmje_delete_subtree(command_list_p->command_p->dm_path_p,
               &(command_list_p->command_p->nv_pair_array),
               result_p,DEL_INSTANCE_ONLY,EXEC_COMPLETE_CMD) != OF_SUCCESS)
               {
               CM_JE_DEBUG_PRINT ("Delete Subtree failed");
               }
               } */
      }
   }

   CM_DLLQ_SCAN(&(UCMJEGloTransaction_g), xtn_entry_p, struct je_xtn_entry*)
   {
      char *dm_path_p = xtn_entry_p->pcDMPath;
      if(xtn_entry_p->status_b  == 1)
      {
         icRet = cm_get_channel_id_and_appl_id (dm_path_p, &appl_id_ui, &channel_id_ui);
         pEndFn = cm_channels_p[channel_id_ui]->cm_end_xtn;
         return_value = pEndFn (appl_id_ui,xtn_entry_p->xtn_rec,CM_CMD_CONFIG_SESSION_REVOKE,&app_result_p);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("End Function returned failure");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_ENDCBK_FAILED,
                  dm_path_p);
            of_free(parent_path_p); //CID 344
            return OF_FAILURE;
         }
         break;
      }
   }
   of_free(parent_path_p); //CID 344
   return OF_SUCCESS;
}






/**
  \ingroup JEAPI

  This deletes all Commands in Configuration Cache and removes configuration session.

  <b>Input paramameters: </b>\n
  <b><i> config_session_p: </i></b> Pointer to Configuration Session for which request
  belongs to 
  <b><i> config_request_p: </i></b> pointer to Configuration Request sent by Management
  Engines.

  <b>Output Parameters: </b>
  <b><i>pUCMResult:</i></b> pointer to UCMResult 
  In Success case it contains data for Get Requests
  In Failure case error information \n

  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_sm_cfgsess_revoke_handler
 ** Description   : This API cleans configuration Cache and closes the Configuration
 **                  Session.
 ** Input params  : config_session_p - Pointer to Configuration Session
 **                 config_request_p - pointer to Configuration Request sent by Management
 **                              Engines.
 ** Output params : result_p - pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t je_sm_cfgsess_revoke_handler (struct je_config_session *
      config_session_p,
      struct je_config_request *
      config_request_p,
      struct cm_result * result_p)
{
   struct je_config_session_cache *pCache = NULL;
   struct je_xtn_entry *xtn_entry_p=NULL, *pTmp=NULL;
   int32_t return_value;


   CM_JE_DEBUG_PRINT ("Entered");
   if (config_session_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("config_session_p is NULL");
      return OF_FAILURE;
   }

   pCache = config_session_p->pCache;
   if (pCache == NULL)
   {
      CM_JE_DEBUG_PRINT ("config_session_p is NULL");
      return OF_FAILURE;
   }

   /** delete all the instances added in the transaction **/
   if((return_value = DelInstances(config_session_p,result_p))!= OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT("deleting instances failed") ;
      return OF_FAILURE;
   }

   /* Freeing All Commands from Cache
    * Free Cache also */
   cmje_cfgsess_clean_cache (pCache);
   cm_je_cfgsession_free (config_session_p);
   CM_INC_STATS_JE (JE_CFGSESSION_CLOSED);
   CM_INC_STATS_JE (JE_CFGSESSION_REVOKED);
   /*Free transaction list*/

   CM_DLLQ_DYN_SCAN (&(UCMJEGloTransaction_g), xtn_entry_p, pTmp, struct je_xtn_entry*)
   {
      if(xtn_entry_p->pcDMPath != NULL)
      {
         of_free(xtn_entry_p->pcDMPath);
         xtn_entry_p->pcDMPath = NULL;
      }
      CM_DLLQ_DELETE_NODE(&UCMJEGloTransaction_g,(UCMDllQNode_t *)xtn_entry_p);
      of_free(xtn_entry_p);
   }
   return OF_SUCCESS;
}

/**
  \ingroup JEAPI

  This API revokes  Configuration session.

  <b>Input paramameters: </b>\n
  <b><i> config_session_p: </i></b> Pointer to Configuration Session for which request
  belongs to 
  <b><i> config_request_p: </i></b> pointer to Configuration Request sent by Management
  Engines.

  <b>Output Parameters: </b>
  <b><i>pUCMResult:</i></b> pointer to UCMResult 
  In Success case it contains data for Get Requests
  In Failure case error information \n

  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_sm_cfgsess_timeout_handler
 ** Description   : This API cleans configuration Cache and closes the Configuration
 **                  Session.
 ** Input params  : config_session_p - Pointer to Configuration Session
 **                 config_request_p - pointer to Configuration Request sent by Management
 **                              Engines.
 ** Output params : result_p - pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t je_sm_cfgsess_timeout_handler (struct je_config_session *
      config_session_p,
      struct je_config_request *
      config_request_p,
      struct cm_result * result_p)
{
   struct je_config_session_cache *pCache = NULL;

   CM_JE_DEBUG_PRINT ("Entered");
   if (config_session_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("config_session_p is NULL");
      return OF_FAILURE;
   }

   pCache = config_session_p->pCache;
   if (pCache == NULL)
   {
      CM_JE_DEBUG_PRINT ("config_session_p is NULL");
      return OF_FAILURE;
   }
   /* Freeing All Commands from Cache
    * Free Cache also */
   cmje_cfgsess_clean_cache (pCache);
   CM_INC_STATS_JE (JE_CFGSESSION_REVOKED);
   /* Free Configuration Session */
   cm_je_cfgsession_free (config_session_p);
   CM_INC_STATS_JE (JE_CFGSESSION_CLOSED);
   return OF_SUCCESS;
}

/**
  \ingroup JEAPI
  This API reads Configuration Session for Managment Engines.
  It gets all configuration Session Info or single name value.

  <b>Input paramameters: </b>\n
  <b><i> config_session_p: </i></b> Pointer to Configuration Session for which request
  belongs to 
  <b><i> config_request_p: </i></b> pointer to Configuration Request sent by Management
  Engines.

  <b>Output Parameters: </b>
  <b><i>pUCMResult:</i></b> pointer to UCMResult 
  In Success case it contains data for Get Requests
  In Failure case error information \n

  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */

/******************************************************************************
 ** Function Name : je_sm_cfgsess_get_handler 
 ** Description   : This API reads Data from Configuration Cache for Management
 **                  Engines
 ** Input params  : config_session_p - Pointer to Configuration Session
 **                 config_request_p - pointer to Configuration Request sent by Management
 **                              Engines.
 ** Output params : result_p - pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/

 int32_t je_sm_cfgsess_get_handler (struct je_config_session * config_session_p,
      struct je_config_request * config_request_p,
      struct cm_result * result_p)
{
   struct je_command_list *command_list_p = NULL;
   uint32_t return_value = 0;
   uint32_t uiNvCnt = 0;
   uint32_t uiCnt = 0;
   uint32_t uiDMPathLen;
   char *dm_path_p = NULL;
   struct cm_array_of_nv_pair *pnv_pair_array = NULL;
   struct cm_array_of_nv_pair *dmpath_key_nv_pairs_p = NULL;
   struct cm_array_of_nv_pair sec_appl_nv_pair_array = { };
   struct cm_array_of_iv_pairs KeysIvArray;
   struct cm_array_of_iv_pairs *result_iv_array_p = NULL;
   cm_getexact_rec_cbk_p pGetExactRecFn;
   struct cm_dm_data_element *pDMNode = NULL;
   uint32_t no_of_commands;
   uint32_t uiCmdIndex = 0;
   uint32_t count_ui;
   uint32_t mgmt_engine_id=0;
   struct cm_array_of_commands *array_of_commands_p;
   UCMDllQNode_t *pDllQNode = NULL;

   CM_JE_DEBUG_PRINT ("Entered");

   if (config_session_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("config_session_p is NULL");
      return OF_FAILURE;
   }
   if (config_session_p->pCache == NULL)
   {
      CM_JE_DEBUG_PRINT ("Configuration Cache is not initialized");
      return OF_FAILURE;
   }
   if (config_request_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("command_p is NULL");
      return OF_FAILURE;
   }


   dm_path_p = config_request_p->dm_path_p;

   mgmt_engine_id = config_request_p->mgmt_engine_id;
#if CM_ROLES_PERM_SUPPORT
   if (cm_verify_role_permissions (dm_path_p, config_request_p->admin_role,
            config_request_p->sub_command_id) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Permission Denied");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_PERMISSION_DENIED,
            dm_path_p);
      return OF_FAILURE;
   }
#endif

   switch (config_request_p->sub_command_id)
   {
      case CM_CMD_GET_CONFIG_SESSION:
         CM_JE_DEBUG_PRINT ("Get Config Session");
         CM_INC_STATS_JESESSION (JE_SESSION_GET_CONFIG_SESSION);
         no_of_commands = config_session_p->pCache->no_of_commands;
         CM_JE_DEBUG_PRINT ("Number of Commands in Session %d", no_of_commands);
         if (no_of_commands <= 0)
         {
            /* Session does not exists with zero commands */
            CM_JE_DEBUG_PRINT ("Session Showing zero commands");
            return OF_FAILURE;
         }
         array_of_commands_p = (struct cm_array_of_commands *) of_calloc (1,
               sizeof
               (struct cm_array_of_commands));
         if (!array_of_commands_p)
         {
            CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Command Array");
            return OF_FAILURE;
         }

         array_of_commands_p->count_ui = no_of_commands;
         array_of_commands_p->Commands = (struct cm_command *) of_calloc
            (array_of_commands_p->count_ui, sizeof (struct cm_command));
         if (!array_of_commands_p->Commands)
         {
            CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Array of Commands");
            of_free (array_of_commands_p);
            return OF_FAILURE;
         }

         /* Travesing Each Cache */
         CM_DLLQ_SCAN (&(config_session_p->pCache->add_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            struct cm_command *command_p = &array_of_commands_p->Commands[uiCmdIndex++];
            count_ui = uiCmdIndex - 1;
            command_p->command_id = CM_CMD_ADD_PARAMS;

            uiDMPathLen = of_strlen (command_list_p->command_p->dm_path_p);
            command_p->dm_path_p = (char *) of_calloc (1, uiDMPathLen + 1);
            if (!command_p->dm_path_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocation Failed for DMPath");
               je_free_array_commands (array_of_commands_p, count_ui);
               return OF_FAILURE;
            }
            of_strncpy (command_p->dm_path_p, command_list_p->command_p->dm_path_p,
                  uiDMPathLen);
            command_p->nv_pair_array.count_ui =
               command_list_p->command_p->nv_pair_array.count_ui;
            command_p->nv_pair_array.nv_pairs =
               (struct cm_nv_pair *) of_calloc (command_p->nv_pair_array.count_ui,
                     sizeof (struct cm_nv_pair));
            if (!command_p->nv_pair_array.nv_pairs)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocation Failed for DMPath");
               of_free (command_p->dm_path_p);
               je_free_array_commands (array_of_commands_p, count_ui);
               return OF_FAILURE;
            }
            for (uiCnt = 0;
                  uiCnt < command_list_p->command_p->nv_pair_array.count_ui; uiCnt++)
            {

               if (je_copy_nvpair_to_nvpair
                     (&command_list_p->command_p->nv_pair_array.nv_pairs[uiCnt],
                      &command_p->nv_pair_array.nv_pairs[uiCnt]) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Copy Nv Pair To Nv Pair Failed");
                  of_free (command_p->dm_path_p);
                  je_free_array_commands (array_of_commands_p, count_ui);
                  return OF_FAILURE;
               }
            }
         }

         /* Travesing Modify Cache */
         CM_DLLQ_SCAN (&(config_session_p->pCache->modify_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            struct cm_command *command_p = &array_of_commands_p->Commands[uiCmdIndex++];
            count_ui = uiCmdIndex - 1;

            command_p->command_id = CM_CMD_SET_PARAMS;

            uiDMPathLen = of_strlen (command_list_p->command_p->dm_path_p);
            command_p->dm_path_p = (char *) of_calloc (1, uiDMPathLen + 1);
            if (!command_p->dm_path_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocation Failed for DMPath");
               je_free_array_commands (array_of_commands_p, count_ui);
               return OF_FAILURE;
            }
            of_strncpy (command_p->dm_path_p, command_list_p->command_p->dm_path_p,
                  uiDMPathLen);

            command_p->nv_pair_array.count_ui =
               command_list_p->command_p->nv_pair_array.count_ui;
            command_p->nv_pair_array.nv_pairs =
               (struct cm_nv_pair *) of_calloc (command_p->nv_pair_array.count_ui,
                     sizeof (struct cm_nv_pair));
            if (!command_p->nv_pair_array.nv_pairs)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Nv Pairs");
               of_free (command_p->dm_path_p);
               je_free_array_commands (array_of_commands_p, count_ui);
               return OF_FAILURE;
            }
            for (uiCnt = 0;
                  uiCnt < command_list_p->command_p->nv_pair_array.count_ui; uiCnt++)
            {

               if (je_copy_nvpair_to_nvpair
                     (&command_list_p->command_p->nv_pair_array.nv_pairs[uiCnt],
                      &command_p->nv_pair_array.nv_pairs[uiCnt]) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Nv Pair To Nv Pair Failed");
                  of_free (command_p->dm_path_p);
                  je_free_array_commands (array_of_commands_p, count_ui);
                  return OF_FAILURE;
               }
            }
         }

         /* Travesing Delete Cache */
         CM_DLLQ_SCAN (&(config_session_p->pCache->delete_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            struct cm_command *command_p = &array_of_commands_p->Commands[uiCmdIndex++];
            count_ui = uiCmdIndex - 1;

            command_p->command_id = CM_CMD_SET_PARAMS;

            uiDMPathLen = of_strlen (command_list_p->command_p->dm_path_p);
            command_p->dm_path_p = (char *) of_calloc (1, uiDMPathLen + 1);
            if (!command_p->dm_path_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocation Failed for DMPath");
               je_free_array_commands (array_of_commands_p, count_ui);
               return OF_FAILURE;
            }
            of_strncpy (command_p->dm_path_p, command_list_p->command_p->dm_path_p,
                  uiDMPathLen);

            command_p->nv_pair_array.count_ui =
               command_list_p->command_p->nv_pair_array.count_ui;
            command_p->nv_pair_array.nv_pairs =
               (struct cm_nv_pair *) of_calloc (command_p->nv_pair_array.count_ui,
                     sizeof (struct cm_nv_pair));
            if (!command_p->nv_pair_array.nv_pairs)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Nv Pairs");
               of_free (command_p->dm_path_p);
               je_free_array_commands (array_of_commands_p, count_ui);
               return OF_FAILURE;
            }
            for (uiCnt = 0;
                  uiCnt < command_list_p->command_p->nv_pair_array.count_ui; uiCnt++)
            {
               if (je_copy_nvpair_to_nvpair
                     (&command_list_p->command_p->nv_pair_array.nv_pairs[uiCnt],
                      &command_p->nv_pair_array.nv_pairs[uiCnt]) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Nv Pair to Nv Pair Failed");
                  of_free (command_p->dm_path_p);
                  je_free_array_commands (array_of_commands_p, count_ui);
                  return OF_FAILURE;
               }
            }
         }
         /* Travesing  Trans Cache */
         CM_DLLQ_SCAN (&(config_session_p->pCache->xtn_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            struct cm_command *command_p = &array_of_commands_p->Commands[uiCmdIndex++];
            count_ui = uiCmdIndex - 1;

            command_p->command_id = CM_CMD_SET_PARAMS;

            uiDMPathLen = of_strlen (command_list_p->command_p->dm_path_p);
            command_p->dm_path_p = (char *) of_calloc (1, uiDMPathLen + 1);
            if (!command_p->dm_path_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocation Failed for DMPath");
               je_free_array_commands (array_of_commands_p, count_ui);
               return OF_FAILURE;
            }
            of_strncpy (command_p->dm_path_p, command_list_p->command_p->dm_path_p,
                  uiDMPathLen);

            command_p->nv_pair_array.count_ui =
               command_list_p->command_p->nv_pair_array.count_ui;
            command_p->nv_pair_array.nv_pairs =
               (struct cm_nv_pair *) of_calloc (command_p->nv_pair_array.count_ui,
                     sizeof (struct cm_nv_pair));
            if (!command_p->nv_pair_array.nv_pairs)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Nv Pairs");
               of_free (command_p->dm_path_p);
               je_free_array_commands (array_of_commands_p, count_ui);
               return OF_FAILURE;
            }
            for (uiCnt = 0;
                  uiCnt < command_list_p->command_p->nv_pair_array.count_ui; uiCnt++)
            {
               if (je_copy_nvpair_to_nvpair
                     (&command_list_p->command_p->nv_pair_array.nv_pairs[uiCnt],
                      &command_p->nv_pair_array.nv_pairs[uiCnt]) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Nv Pair to Nv Pair Failed");
                  of_free (command_p->dm_path_p);
                  je_free_array_commands (array_of_commands_p, count_ui);
                  return OF_FAILURE;
               }
            }
         }
         result_p->data.je_data_p = (void *) array_of_commands_p;
         break;

      case CM_CMD_COMPARE_PARAM:
         CM_JE_DEBUG_PRINT ("Handling Compare Param Command");
         CM_INC_STATS_JESESSION (JE_SESSION_COMPARE_PARAM);
         CM_DLLQ_SCAN (&(config_session_p->pCache->add_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            if (!of_strcmp (command_list_p->command_p->dm_path_p, config_request_p->dm_path_p))
            {
               for (uiCnt = 0;
                     uiCnt < command_list_p->command_p->nv_pair_array.count_ui; uiCnt++)
               {
                  if (!of_strcmp
                        (config_request_p->data.nv_pair_array.nv_pairs[0].name_p,
                         command_list_p->command_p->nv_pair_array.nv_pairs[uiCnt].name_p))
                  {
                     CM_JE_DEBUG_PRINT ("NvPair Found in Add Cache of Session");
                     pnv_pair_array = (struct cm_array_of_nv_pair *) of_calloc
                        (1, sizeof (struct cm_array_of_nv_pair));
                     if (!pnv_pair_array)
                     {
                        CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
                        return OF_FAILURE;
                     }
                     /* Only 1 Nv Pair will be present */
                     pnv_pair_array->count_ui = 1;
                     pnv_pair_array->nv_pairs = (struct cm_nv_pair *) of_calloc
                        (pnv_pair_array->count_ui, sizeof (struct cm_nv_pair));
                     if (!pnv_pair_array->nv_pairs)
                     {
                        CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
                        of_free (pnv_pair_array);
                        return OF_FAILURE;
                     }
                     if (je_copy_nvpair_to_nvpair
                           (&command_list_p->command_p->nv_pair_array.nv_pairs[uiCnt],
                            &pnv_pair_array->nv_pairs[0]) != OF_SUCCESS)
                     {
                        CM_JE_DEBUG_PRINT ("Nv Pair To Nv Pair Failed");
                        of_free (pnv_pair_array);
                        return OF_FAILURE;
                     }

                     CM_JE_DEBUG_PRINT ("Value in Session is %s",
                           (char *) pnv_pair_array->nv_pairs[0].value_p);
                     result_p->data.je_data_p = (void *) pnv_pair_array;
                     return OF_SUCCESS;
                  }
               }
            }
         }

         CM_DLLQ_SCAN (&(config_session_p->pCache->modify_config_list), pDllQNode,
               UCMDllQNode_t *)
         {
            command_list_p =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
            if (!of_strcmp (command_list_p->command_p->dm_path_p, config_request_p->dm_path_p))
            {
               for (uiCnt = 0;
                     uiCnt < command_list_p->command_p->nv_pair_array.count_ui; uiCnt++)
               {
                  if (!of_strcmp
                        (config_request_p->data.nv_pair_array.nv_pairs[0].name_p,
                         command_list_p->command_p->nv_pair_array.nv_pairs[uiCnt].name_p))
                  {
                     CM_JE_DEBUG_PRINT ("NvPair Found in Modify Cache of Session");

                     pnv_pair_array = (struct cm_array_of_nv_pair *) of_calloc
                        (1, sizeof (struct cm_array_of_nv_pair));
                     if (!pnv_pair_array)
                     {
                        CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
                        return OF_FAILURE;
                     }
                     pnv_pair_array->count_ui = 2;
                     pnv_pair_array->nv_pairs = (struct cm_nv_pair *) of_calloc
                        (pnv_pair_array->count_ui, sizeof (struct cm_nv_pair));
                     if (!pnv_pair_array->nv_pairs)
                     {
                        CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
                        of_free (pnv_pair_array);
                        return OF_FAILURE;
                     }
                     if (je_copy_nvpair_to_nvpair
                           (&command_list_p->command_p->nv_pair_array.nv_pairs[uiCnt],
                            &pnv_pair_array->nv_pairs[0]) != OF_SUCCESS)
                     {
                        CM_JE_DEBUG_PRINT ("Nv Pair to Nv Pair Failed");
                        of_free (pnv_pair_array);
                        return OF_FAILURE;
                     }
                     CM_JE_DEBUG_PRINT ("Value in Session is %s",
                           (char *) pnv_pair_array->nv_pairs[0].value_p);
                     pDMNode =
                        (struct cm_dm_data_element *)
                        cmi_dm_get_dm_node_from_instance_path (dm_path_p, of_strlen (dm_path_p));
                     if ((!pDMNode) && (pDMNode->app_cbks_p))
                     {
                        CM_JE_DEBUG_PRINT ("pDMNode is NULL");
                        UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                              UCMJE_ERROR_GETNODE_BYDMPATH,
                              dm_path_p);
                        cm_je_free_ptr_nvpairArray (pnv_pair_array);
                        return OF_FAILURE;
                     }

                     pGetExactRecFn = pDMNode->app_cbks_p->cm_getexact_rec;
                     if (!pGetExactRecFn)
                     {
                        CM_JE_DEBUG_PRINT ("GETExactRecord Callback is NULL");
                        UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                              UCMJE_ERROR_GETNODE_BYDMPATH,
                              dm_path_p);
                        cm_je_free_ptr_nvpairArray (pnv_pair_array);
                        return OF_FAILURE;
                     }

                     if ((return_value = cmi_dm_get_key_array_from_namepath (dm_path_p,
                                 &dmpath_key_nv_pairs_p))
                           != OF_SUCCESS)
                     {
                        if(return_value == OF_FAILURE)
                        {
                           return_value = UCMJE_ERROR_DMGET_KEYS_FROM_NAMEPATH_FAILED;
                        }
                        CM_JE_DEBUG_PRINT ("Get Keys Array From Name Path failed");
                        UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                              return_value, dm_path_p);
                        cm_je_free_ptr_nvpairArray (pnv_pair_array);
                        return OF_FAILURE;
                     }

                     /* Copy Input Keys Array */
                     if (je_copy_nvpairarr_to_ivpairarr (dm_path_p,
                              dmpath_key_nv_pairs_p,
                              &KeysIvArray,
                              mgmt_engine_id) != OF_SUCCESS)
                     {
                        CM_JE_DEBUG_PRINT ("Copy NvPair Arr to IVPairArr Failed");
                        UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                              UCMJE_ERROR_GETNODE_BYDMPATH,
                              dm_path_p);
                        cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
                        cm_je_free_ptr_nvpairArray (pnv_pair_array);
                        return OF_FAILURE;
                     }

                     return_value = pGetExactRecFn (&KeysIvArray, &result_iv_array_p);
                     /* Convert Result IvPairs into nv_pairs */
                     if (je_copy_ivpairarr_to_nvpairarr (dm_path_p,
                              result_iv_array_p,
                              &sec_appl_nv_pair_array) !=
                           OF_SUCCESS)
                     {
                        CM_JE_DEBUG_PRINT ("Copy NvPair Arr to IVPairArr Failed");
                        UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                              UCMJE_ERROR_GETNODE_BYDMPATH,
                              dm_path_p);
                        cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
                        cm_je_free_ivpair_array (&KeysIvArray);
                        cm_je_free_ptr_ivpairArray (result_iv_array_p);
                        cm_je_free_ptr_nvpairArray (pnv_pair_array);
                        return OF_FAILURE;
                     }

                     for (uiNvCnt = 0; uiNvCnt < sec_appl_nv_pair_array.count_ui; uiNvCnt++)
                     {
                        if (!of_strcmp
                              (sec_appl_nv_pair_array.nv_pairs[uiNvCnt].name_p,
                               config_request_p->data.nv_pair_array.nv_pairs[0].name_p))
                        {
                           if (je_copy_nvpair_to_nvpair (&sec_appl_nv_pair_array.nv_pairs
                                    [uiNvCnt],
                                    &pnv_pair_array->nv_pairs[1])
                                 != OF_SUCCESS)
                           {
                              CM_JE_DEBUG_PRINT ("Nv Pair to Nv Pair Failed");
                              UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                                    UCMJE_ERROR_GETNODE_BYDMPATH,
                                    dm_path_p);
                              cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
                              cm_je_free_ivpair_array (&KeysIvArray);
                              cm_je_free_ptr_ivpairArray (result_iv_array_p);
                              cm_je_free_ptr_nvpairArray (pnv_pair_array);
                              return OF_FAILURE;
                           }

                           CM_JE_DEBUG_PRINT ("Value from security application is %s",
                                 (char *) pnv_pair_array->
                                 nv_pairs[1].value_p);
                        }
                     }
                     /* output assignement */
                     result_p->data.je_data_p = (void *) pnv_pair_array;
                     cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
                     cm_je_free_ivpair_array (&KeysIvArray);
                     cm_je_free_ptr_ivpairArray (result_iv_array_p);
                     je_free_nvpair_array (&sec_appl_nv_pair_array);
                     return OF_SUCCESS;
                  }

               }
            }
         }
         return OF_FAILURE;
      default:
         CM_JE_DEBUG_PRINT ("Un handled command");
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
            (result_p, UCMJE_ERROR_UNKNOWN_SUB_COMMAND, dm_path_p);
         return OF_FAILURE;
   }
   return OF_SUCCESS;
}

/**
  \ingroup JEAPI

  This API Allocates New Configuration Session and Initializes it.
  <b>Input paramameters: </b>\n
  <b><i> config_request_p: </i></b> pointer to Configuration Request sent by Management
  Engines.

  <b>Output Parameters: </b> None \n
  <b>Return Value:</b>  Pointer to Configuration Session  in Success case OR
  NULL in Failure case \n.
  */
/******************************************************************************
 ** Function Name : cmje_config_session_start
 ** Description   : This API Allocates New Configuration Session and Initializes it
 ** Input params  : config_request_p - pointer to Configuration Request sent by Management
 **                  Engines.
 ** Output params : None
 ** Return value  : Pointer to Configuration Session in Success case OR
 **                  NULL in Failure case
 *******************************************************************************/
struct je_config_session *cmje_config_session_start (struct je_config_request *
      config_request_p)
{
   struct je_config_session *pConfigSess = NULL;
   uint32_t uiNewId = 0;

   CM_JE_DEBUG_PRINT ("Entered");

   if (config_request_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid Input");
      return NULL;
   }

   pConfigSess = (struct je_config_session *) cm_je_cfgsession_alloc ();
   if (pConfigSess == NULL)
   {
      CM_JE_DEBUG_PRINT ("config_session_p is NULL");
      return NULL;
   }

   /* Generate unique sessionID */
   cmje_generate_session_id (&uiNewId);

   /* Initialize Session Values */
   pConfigSess->session_id = uiNewId;
   pConfigSess->mgmt_engine_id = config_request_p->mgmt_engine_id;
   of_strncpy (pConfigSess->admin_role, config_request_p->admin_role,
         CM_MAX_ADMINROLE_LEN);
   of_strncpy (pConfigSess->admin_name, config_request_p->admin_name,
         CM_MAX_ADMINNAME_LEN);
   pConfigSess->flags = config_request_p->flags;
   pConfigSess->state = UCMJE_CFGSESS_ACTIVE_STATE;

   /* Allocate Memory for Configuration Cache */
   pConfigSess->pCache = (struct je_config_session_cache *) cm_je_cfgcache_alloc ();
   if (pConfigSess->pCache == NULL)
   {
      CM_JE_DEBUG_PRINT ("config_session_p is NULL");
      cm_je_cfgsession_free (pConfigSess);
      return NULL;
   }

   /* Initialize Configuration Cache */
   pConfigSess->pCache->no_of_commands = 0;
   CM_DLLQ_INIT_LIST (&(pConfigSess->pCache->add_config_list));
   CM_DLLQ_INIT_LIST (&(pConfigSess->pCache->modify_config_list));
   CM_DLLQ_INIT_LIST (&(pConfigSess->pCache->delete_config_list));
   CM_DLLQ_INIT_LIST (&(pConfigSess->pCache->xtn_config_list));

   CM_JE_DEBUG_PRINT ("SessionId=%d MgmtEngine=%d AdminName=%s AdminRole=%s",
         pConfigSess->session_id,
         pConfigSess->mgmt_engine_id,
         pConfigSess->admin_name, pConfigSess->admin_role);
   CM_INC_STATS_JE (JE_CFGSESSION_STARTED);
   return pConfigSess;
}

/**
  \ingroup JEAPI
  This API Generates Unique Session ID for a Configuration Session
  <b>Input paramameters: </b> NONE \n
  <b>Output Parameters: </b>
  <b><i>pNewID:</i></b> pointer to New Session ID
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cmje_generate_session_id 
 ** Description   : This API Generates a Unique Session ID for a Configuration Session
 ** Input params  : None
 ** Output params :  pNewID - Pointer to New Session ID
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cmje_generate_session_id (uint32_t * pNewID)
{
   if (uiJEsession_id_g == 0)
      uiJEsession_id_g = UCMJE_SESSION_BASEID;
   else if (uiJEsession_id_g == UCMJE_SESSION_MAXID)
      uiJEsession_id_g = UCMJE_SESSION_BASEID;
   else
   {
      uiJEsession_id_g++;
   }

   *pNewID = uiJEsession_id_g;
   CM_JE_DEBUG_PRINT ("uiJEsession_id_g =%u *pNewID=%u ", uiJEsession_id_g,
         *pNewID);
   return OF_SUCCESS;
}

/**
  \ingroup JEAPI
  This API Delete and deallocate each command in Configuration Session Cache.
  <b>Input paramameters: </b>\n
  <b><i>pCfgCache:</i></b> pointer to New Session ID
  <b>Output Parameters: </b> None \n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cmje_cfgsess_clean_cache 
 ** Description   : This API Delete and deallocate each command in Configuration 
 Session Cache
 ** Input params  : pCfgCachem - Pointer to Configuration Cache structure
 ** Output params : None
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cmje_cfgsess_clean_cache (struct je_config_session_cache * pCfgCache)
{
   struct je_command_list *command_list_p = NULL, *pTmp = NULL;

   if (pCfgCache == NULL)
   {
      CM_JE_DEBUG_PRINT ("config_session_p is NULL");
      return OF_FAILURE;
   }

   CM_DLLQ_DYN_SCAN (&(pCfgCache->add_config_list), command_list_p, pTmp,
         struct je_command_list *)
   {
      if (command_list_p)
      {
         CM_DLLQ_DELETE_NODE (&(pCfgCache->add_config_list),
               (UCMDllQNode_t *) command_list_p);
         je_free_command_list (command_list_p);
         pCfgCache->no_of_commands--;
      }
   }

   CM_DLLQ_DYN_SCAN (&(pCfgCache->modify_config_list), command_list_p, pTmp,
         struct je_command_list *)
   {
      if (command_list_p)
      {
         CM_DLLQ_DELETE_NODE (&(pCfgCache->modify_config_list),
               (UCMDllQNode_t *) command_list_p);
         je_free_command_list (command_list_p);
         pCfgCache->no_of_commands--;
      }
   }

   CM_DLLQ_DYN_SCAN (&(pCfgCache->delete_config_list), command_list_p, pTmp,
         struct je_command_list *)
   {
      if (command_list_p)
      {
         CM_DLLQ_DELETE_NODE (&(pCfgCache->delete_config_list),
               (UCMDllQNode_t *) command_list_p);
         je_free_command_list (command_list_p);
         pCfgCache->no_of_commands--;
      }
   }

   CM_DLLQ_DYN_SCAN (&(pCfgCache->xtn_config_list), command_list_p, pTmp,
         struct je_command_list *)
   {
      if (command_list_p)
      {
         CM_DLLQ_DELETE_NODE (&(pCfgCache->xtn_config_list),
               (UCMDllQNode_t *) command_list_p);
         je_free_command_list (command_list_p);
         pCfgCache->no_of_commands--;
      }
   }

   CM_JE_DEBUG_PRINT ("Commands remained in cache  = %d after cleaning",
         pCfgCache->no_of_commands);
   cm_je_cfgcache_free (pCfgCache);
   return OF_SUCCESS;
}

/**
  \ingroup JEAPI
  This API Validates a Command and Name,Value pairs before adding it to Configuration
  Cache. 
  For each Name in Name Value Pair Array it gets Data Model Attributes using Data Model
  Path.
  <b>Input paramameters: </b>\n
  <b><i>command_id:</i></b> Command ID Value from following
  CM_CMD_ADD_PARAMS/CM_CMD_SET_PARAMS/CM_CMD_DEL_PARAMS
  <b><i>dm_path_p</i></b>  Character Pointer to Data Model Path
  <b><i>nv_pair_array</i></b> Array of Name Value Pairs  
  <b>Output Parameters: </b>
  <b><i>result_p</i></b> double pointer to UCMResult
  In Success case it contains data and in  Failure case error information  
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_validate_command 
 ** Description   : This API Validates Command for a Given Data Model Path and each
 Name Value pair using its Data Model Attributes.
 ** Input params  : command_id - Command ID Value
 CM_CMD_ADD_PARAMS or
 CM_CMD_SET_PARAMS or
 CM_CMD_DEL_PARAMS
 dm_path_p   - Character Pointer to Data Model Path
 nv_pair_array - Array of Name Value Pairs
 ** Output params : result_p - double pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t je_validate_command (uint32_t command_id,
      char * dm_path_p,
      struct cm_array_of_nv_pair nv_pair_array,
      struct cm_result * result_p)
{
   int32_t uiNvCnt;
   struct cm_dm_data_element *pDMParentNode = NULL;
#ifdef CM_JE_CHECK_MAX_TABLES
   UCMDM_Inode_t *pInode;
   UCMDllQ_t *pDll;
   uint32_t uiInstanceCnt, uiMaxRows;
   char * parent_path_p;
   uint32_t uiPathLen = 0;
#endif

   if (!dm_path_p || of_strlen(dm_path_p) == 0)
   {
      CM_JE_DEBUG_PRINT ("DM Path is NULL");
      result_p->result.error.error_code = UCMJE_ERROR_DMPATH_NULL;
      return OF_FAILURE;
   }

   pDMParentNode = (struct cm_dm_data_element *)
      cmi_dm_get_dm_node_from_instance_path (dm_path_p, of_strlen (dm_path_p));
   if (!pDMParentNode)
   {
      CM_JE_DEBUG_PRINT ("pDMParentNode is NULL");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_GETNODE_BYDMPATH,
            dm_path_p);
      return OF_FAILURE;
   }

#ifdef CM_JE_CHECK_MAX_TABLES
   if (command_id == CM_CMD_ADD_PARAMS)
   {
      uiMaxRows = pDMParentNode->data_attrib.attr.max_no_of_tables;
      /* First get the Inode matching the path */
      uiPathLen = of_strlen(dm_path_p);
      parent_path_p = (char *)of_calloc(1, uiPathLen+1);
      cm_remove_last_node_instance_from_dmpath(dm_path_p, parent_path_p);
      pInode = cmi_dm_get_inode_by_name_path (parent_path_p);
      if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
      {
         CM_JE_DEBUG_PRINT ("GetInode Failed");
         of_free(parent_path_p);
				 goto After_TableCheck;
      }
      CM_JE_DEBUG_PRINT ("Max Tables %d for %s", uiMaxRows, dm_path_p);
      pDll = &pInode->ChildAccess.InstanceList;
      uiInstanceCnt = CM_DLLQ_COUNT (pDll);
      if ((uiInstanceCnt > 0) && (uiInstanceCnt == uiMaxRows))
      {
         CM_JE_DEBUG_PRINT ("Instance Count reached to max limit %d ",
               uiInstanceCnt);
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
               UCMJE_DMERROR_MAX_INSTANCES_REACHED,
               dm_path_p);
         of_free(parent_path_p);
         return OF_FAILURE;
      }
      of_free(parent_path_p);
   }
After_TableCheck:
#endif
   for (uiNvCnt = 0; uiNvCnt < nv_pair_array.count_ui; uiNvCnt++)
   {
      char *name_p = nv_pair_array.nv_pairs[uiNvCnt].name_p;
      struct cm_dm_data_element *pDMChildNode = NULL;

      if(name_p && of_strcmp(name_p,CM_DM_ELEMENT_BPASSIV_KEY_NAME) != 0)
      {
         pDMChildNode =
            (struct cm_dm_data_element *) cmi_dm_get_child_by_name (pDMParentNode, name_p);
         if (pDMChildNode == NULL)
         {
            CM_JE_DEBUG_PRINT ("GetChildByName %s for Parent %s Failed", name_p,
                  pDMParentNode->name_p);
            if (!of_strcmp (name_p, CM_DM_ELEMENT_RELATIVITY_NAME))
            {
               pDMChildNode = &RelativityNode;
            }
            else if (!of_strcmp (name_p, CM_DM_ELEMENT_RELATIVE_KEY_NAME))
            {
               pDMChildNode = cmi_dm_get_key_child_element (pDMParentNode);
            }
            else if (pDMParentNode->element_attrib.element_type ==
                  CM_DMNODE_TYPE_SCALAR_VAR)
            {
               if ((nv_pair_array.count_ui == 1))
               {
                  if (!of_strcmp (name_p, pDMParentNode->name_p))
                  {
#ifdef CM_JE_SEARCH_CHILD_IN_COMPLETE_DMPATH
                     CM_JE_DEBUG_PRINT
                        ("It is a individual scalar variable to configure...so retrying");
                     pDMChildNode = cmi_dm_get_child_node_by_name_from_dm_path (dm_path_p, name_p);
#endif
                  }
               }
               else
               {
                  CM_JE_DEBUG_PRINT
                     ("scalar variables should not configure more than one field");
               }
            }
            if (pDMChildNode == NULL)
            {
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                     UCMJE_ERROR_GETCHILD_BYNAME,
                     dm_path_p);
               if (je_copy_nvpair_to_nvpair (&nv_pair_array.nv_pairs[uiNvCnt],
                        &result_p->result.error.nv_pair)
                     != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Nv Pair To Nv Pair Failed");
               }
               else
                  result_p->result.error.nv_pair_b = TRUE;
               return UCMJE_ERROR_VALIDATION_FAILED;
            }
         }
         /* Compare Command and Cancel Previous Commands 
          * do not required to Validate Attributes */
         if (!((command_id == CM_CMD_COMPARE_PARAM) ||
                  (command_id == CM_CMD_CANCEL_PREV_CMD)))
         {
            if ((je_validate_attribute (pDMChildNode,
                        &(nv_pair_array.nv_pairs[uiNvCnt]),
                        result_p)) != OF_SUCCESS)
            {
               /* Error Code already copied in ValidateAttribute function */
               UCMJE_MEMALLOC_AND_COPY_ERROR_DMPATH_INTO_UCMRESULT (result_p, dm_path_p);
               return UCMJE_ERROR_VALIDATION_FAILED;
            }
         }
      }
   }
   return OF_SUCCESS;
}

/**
  \ingroup JEAPI
  This API used to Verification of any referrences for the commands in given 
  Configuration Session.
  This API checks given Data Model Path has any reference by invoking Data Model API.
  This API checks the reference instance for the given DM Path already present in Data
  Model Instance tree by invoking DATA Model API.
  If reference is not present in DATA Model instance tree, API checks any “add” command
  for the same in configuration cache.
  If referenced command  exists , 
  i). If it is before the referring command in Configuration Cache, It simply enables
  referrence_in_cache_b flag for the referring command.
  ii). If it is present after the referring command in Configuration Cache, It re-orders
  the Command List and enables referrence_in_cache_b flag for the referring command.

  <b>Input paramameters: </b>\n
  <b><i>config_session_p:</i></b> Pointer to Configuration Session
  <b>Output Parameters: </b>
  <b><i>result_p</i></b> double pointer to UCMResult
  In Success case None and in  Failure case error information  
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_verify_command_referrence
 ** Description   : This API checks for any referrences for the command in Configuration
 Session and re-arranges its list.
 ** Input params  : config_session_p - Pointer to Configuration Session
 ** Output params : result_p - double pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t je_verify_command_referrence (struct je_config_session *
      config_session_p,
      struct cm_result * result_p)
{
   struct je_command_list *command_list_p = NULL;
   int32_t return_value;
   UCMDllQNode_t *pDllQNode = NULL;

   CM_JE_DEBUG_PRINT ("Entered");

   if (config_session_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("config_session_p is NULL");
      return OF_FAILURE;
   }
   if (config_session_p->pCache == NULL)
   {
      CM_JE_DEBUG_PRINT ("Configuration Cache is not initialized");
      return OF_FAILURE;
   }

   if((return_value = je_verify_referrence(&(config_session_p->pCache->add_config_list),result_p))!= OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT("Reference validation failed");
      return OF_FAILURE;
   }

   if((return_value = je_verify_referrence(&(config_session_p->pCache->xtn_config_list),result_p))!= OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT("Reference validation failed");
      return OF_FAILURE;
   }

   /* Resetting Flag to False
    * Flag can be used for other purpose
    */
   CM_DLLQ_SCAN (&(config_session_p->pCache->add_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      if (command_list_p && command_list_p->command_p)
      {
         command_list_p->success_b = FALSE;
      }
   }

   CM_DLLQ_SCAN (&(config_session_p->pCache->xtn_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      if (command_list_p && command_list_p->command_p)
      {
         command_list_p->success_b = FALSE;
      }
   }
   return OF_SUCCESS;
}

 int32_t je_verify_referrence (UCMDllQ_t *pCfgList,
      struct cm_result * result_p)
{
   struct je_command_list *command_list_p = NULL;
   struct je_command_list *command_list_tmp_p = NULL;
   uint32_t uiRefCnt;
   uint32_t uiNvCnt;
   uint32_t uiDMPathLen = 0;
   char *pDMTplPath, *dm_path_p;
   struct cm_dm_data_element *parent_node_p;
   struct cm_dm_data_element *pNode;
   UCMDllQNode_t *pDllQNode = NULL, *pDllQNode2 = NULL;

   do
   {
      CM_DLLQ_SCAN (pCfgList, pDllQNode, UCMDllQNode_t *)
      {
         command_list_p =
            CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
         if (command_list_p && command_list_p->command_p
               && (command_list_p->success_b != TRUE))
         {
            /* Allocate token buffer */
            dm_path_p = command_list_p->command_p->dm_path_p;
            uiDMPathLen = of_strlen (command_list_p->command_p->dm_path_p) + 1;
            pDMTplPath = (char *) of_calloc (1, uiDMPathLen);

            CM_JE_DEBUG_PRINT ("Checking referrences for DM PATH  %s ", dm_path_p);
            if (!pDMTplPath)
            {
               CM_JE_DEBUG_PRINT ("MemoryAllocation failed for Template Path");
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
                  (result_p, UCMJE_ERROR_MEMORY_ALLOCATION_FAILED, dm_path_p);
               return OF_FAILURE;
            }

            /* Verify whether the data path is valid accroding to DMTree */
            parent_node_p = (struct cm_dm_data_element *)
               cmi_dm_get_dm_node_from_instance_path (command_list_p->command_p->dm_path_p,
                     of_strlen (command_list_p->command_p->
                        dm_path_p));

            if (unlikely (!parent_node_p))
            {
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
                  (result_p, UCMJE_ERROR_GETNODE_BYDMPATH, dm_path_p);
               of_free (pDMTplPath);
               return OF_FAILURE;
            }

            /* Check each NVPair */
            for (uiNvCnt = 0; uiNvCnt <
                  command_list_p->command_p->nv_pair_array.count_ui; uiNvCnt++)
            {
               struct cm_nv_pair *pNvPair =
                  &command_list_p->command_p->nv_pair_array.nv_pairs[uiNvCnt];

               if ((!of_strcmp (pNvPair->name_p, CM_DM_ELEMENT_RELATIVITY_NAME)) ||
                     (!of_strcmp (pNvPair->name_p, CM_DM_ELEMENT_RELATIVE_KEY_NAME)))
               {
                  continue;
               }
               pNode =
                  (struct cm_dm_data_element *) cmi_dm_get_child_by_name (parent_node_p,
                        pNvPair->name_p);
               if (unlikely (!pNode))
               {
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
                     (result_p, UCMJE_ERROR_GETCHILD_BYNAME, dm_path_p);
                  UCMJE_COPY_PTR_NVPAIR_TO_NVPAIR (pNvPair,
                        result_p->result.error.nv_pair);
                  of_free (pDMTplPath);
                  return OF_FAILURE;
               }

               /* referrence are preset ?? */
               if ((pNode->ref_names_p) && (pNode->ref_names_p->count_ui > 0))
               {
                  /* Read Each Referrence */
                  for (uiRefCnt = 0; uiRefCnt < pNode->ref_names_p->count_ui; uiRefCnt++)
                  {
                     char *pRefEleName = NULL;
                     unsigned char bSameNodeFound = FALSE;
                     unsigned char referrence_in_cache_b = FALSE;
                     char aTemp[CM_JE_MAX_REFFERENCE_LEN] = { };
                     char *pNewRefPath = NULL;

                     of_strcpy (aTemp, pNode->ref_names_p->string_arr[uiRefCnt]);
                     /* Get Only Node Name from complete path */
                     UCMJE_ELEMENT_NAME_FROM_REFERRENCE (aTemp, pRefEleName);
                     CM_JE_DEBUG_PRINT ("Node %s has a referrence %s", pNode->name_p,
                           pRefEleName);

                     /* Make referrence path same in DM Node path */
                     pNewRefPath =
                        (char *) cmi_dm_make_instance_path (command_list_p->
                              command_p->dm_path_p,
                              pNode->
                              ref_names_p->string_arr
                              [uiRefCnt]);
                     if (pNewRefPath == NULL)
                     {
                        CM_JE_DEBUG_PRINT ("MakeInstancePath failed for %s",
                              command_list_p->command_p->dm_path_p);
                        UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
                           (result_p,
                            UCMJE_ERROR_CREATE_INSTANCEPATH_FROM_REFERRENCE_FAILED,
                            dm_path_p);
                        UCMJE_COPY_PTR_NVPAIR_TO_NVPAIR (pNvPair,
                              result_p->result.
                              error.nv_pair);
                        of_free (pDMTplPath);
                        return OF_FAILURE;
                     }

                     CM_JE_DEBUG_PRINT ("New Instance Path %s", pNewRefPath);
                     if (cmi_dm_is_instance_map_entry_using_key_and_name_path (pNewRefPath,
                              pNvPair->value_p,
                              pNvPair->
                              value_type) ==
                           TRUE)
                     {
                        CM_JE_DEBUG_PRINT
                           ("Instance present in DM Instance Tree for %s", pNewRefPath);
                        /* Instance Found for this referrence 
                         * Check for next referrence
                         */
                        of_free (pNewRefPath);
                        continue;
                     }

                     CM_JE_DEBUG_PRINT
                        ("Instance Not found in DM Instance Tree for %s", pNewRefPath);

                     /* Referrence not present in DM Instance Tree 
                      * Check any request for referrence is already added in same Session?? */
                     CM_DLLQ_SCAN (pCfgList, pDllQNode2,
                           UCMDllQNode_t *)
                     {
                        command_list_tmp_p =
                           CM_DLLQ_LIST_MEMBER (pDllQNode2, struct je_command_list *, list_node);
                        if (!of_strcmp
                              (command_list_p->command_p->dm_path_p,
                               command_list_tmp_p->command_p->dm_path_p))
                        {
                           bSameNodeFound = TRUE;
                        }

                        if (!of_strcmp (pNewRefPath, command_list_tmp_p->command_p->dm_path_p))
                        {
                           /* Referrence found for command_list_p */
                           command_list_p->referrence_in_cache_b = TRUE;
                           referrence_in_cache_b = TRUE;
                           CM_JE_DEBUG_PRINT
                              ("Referrence Command present in Config Session");
                           if (bSameNodeFound == TRUE)
                           {
                              /* Referrence Present in Command List2
                               * It has to be executed before Command List1
                               * change the DLLQ 
                               */
                              CM_DLLQ_DELETE_NODE (pCfgList,
                                    (UCMDllQNode_t *) command_list_tmp_p);
                              CM_DLLQ_INSERT_BEFORE_NODE (pCfgList,
                                    (UCMDllQNode_t *)
                                    command_list_p,
                                    (UCMDllQNode_t *)
                                    command_list_tmp_p);

                              CM_JE_DEBUG_PRINT
                                 ("Re-ordering commands into DMPath %s, DMPath %s",
                                  command_list_tmp_p->command_p->dm_path_p,
                                  command_list_p->command_p->dm_path_p);
                           }
                        }
                     }                 /* End of CM_DLL_SCAN2 */
                     if (referrence_in_cache_b == FALSE)
                     {
                        CM_JE_DEBUG_PRINT
                           ("Referrence Not found in Cache and in Session list");
                        UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                              UCMJE_ERROR_REFERRENCE_NOT_FOUND,
                              dm_path_p);
                        UCMJE_COPY_PTR_NVPAIR_TO_NVPAIR (pNvPair,
                              result_p->result.
                              error.nv_pair);
                        of_free (pDMTplPath);
                        of_free (pNewRefPath);
                        return OF_FAILURE;
                     }
                     of_free (pNewRefPath);
                  }                   // read each referrence
               }                     // referrences are present for NvPair     
            }                       //read each NvPair from command list

            of_free (pDMTplPath);
            pDMTplPath = NULL;

            /* Succesfully scanned */
            command_list_p->success_b = TRUE;
            if (command_list_p->referrence_in_cache_b == TRUE)
            {
               /*DLL Order might be changed
                * Scan it from beginning
                */
               break;
            }
         }
      }                           /* SCAN */
   }while (pDllQNode);
   return OF_SUCCESS;
}


/**
  \ingroup JEAPI
  This API Validates a given Name Value Pair using its Data Model Element Attribute
  <b>Input paramameters: </b>\n
  <b><i>attrib_p:</i></b> Pointer to Data Model Element attribute
  <b><i>pNvPair</i></b>  Pointer to Name Value Pair
  <b>Output Parameters: </b>
  <b><i>result_p</i></b> double pointer to UCMResult
  In Success case it contains data and in  Failure case error information  
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_validate_attribute
 ** Description   : This API Validates Name Value Pair using Data Model Element
 Attribute
 ** Input params  : attrib_p  - Pointer to Data Model Element attribute
 pNvPair  - Pointer to Name and Value Pair
 ** Output params : result_p - double pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t je_validate_attribute (struct cm_dm_data_element * attrib_p,
      struct cm_nv_pair * pNvPair,
      struct cm_result * result_p)
{
   uint32_t uiCnt = 0;
   uint32_t uiStrLen = 0;
   uint32_t uiValNum = 0;
   int32_t iValNum = 0;
   int32_t error_code = 0;
   char *name_p = NULL;
   char *value_p = NULL;
   char *pToken = NULL;
   uint32_t uiValLen = 0;
   unsigned char bMatchFound = FALSE;
   uint32_t ulIp = 0;
   uint32_t ulStartIp;
   uint32_t ulEndIp;
   unsigned char bNonEdit = FALSE;

#ifdef CM_IPV6_SUPPORT

   struct in6_addr ulIpv6;
   struct in6_addr ulEndIpv6;
   struct in6_addr ulStartIpv6;
   unsigned char bStart = FALSE;
   unsigned char bEnd = FALSE;
   int32_t ii = 0;

#endif /*IPV6 support */

   uint32_t uiResultLen;
   char aTmpBuf[CM_JE_ERROR_STRING_LEN];

   if (!pNvPair)
   {
      CM_JE_DEBUG_PRINT ("Name Value Pair is NULL");
      error_code = UCMJE_DATA_ELEM_VALUE_NULL;
      goto ErrorHandler;
   }
   name_p = pNvPair->name_p;
   if (!name_p)
   {
      CM_JE_DEBUG_PRINT ("Name of the field is NULL");
      error_code = UCMJE_DATA_ELEM_NAME_NULL;
      goto ErrorHandler;
   }
   value_p = pNvPair->value_p;
   if (!value_p)
   {
      CM_JE_DEBUG_PRINT ("Value  of the field is NULL");
      error_code = UCMJE_DATA_ELEM_VALUE_NULL;
      goto ErrorHandler;
   }
   uiValLen = of_strlen (value_p);
#if 0
   if((attrib_p->element_attrib.non_config_leaf_node_b == TRUE)&&
         (attrib_p->element_attrib.element_type == CM_DMNODE_TYPE_SCALAR_VAR))
   {
      bNonEdit = TRUE;
   }
#endif

   switch (attrib_p->data_attrib.data_type)
   {
      case CM_DATA_TYPE_INT:
         if (!ucmJEIsDigits ((char *) value_p, uiValLen))
         {
            error_code = UCMJE_DATA_ELEM_VALTYPE_INVALID_INT;
            goto ErrorHandler;
         }
         iValNum = of_atoi (value_p);
         if( (bNonEdit == TRUE) &&
               (iValNum != attrib_p->data_attrib.default_value.idefault_value) )
         {
            CM_JE_DEBUG_PRINT ("Key does not have permissions to edit");
            error_code = UCMJE_ERROR_PERMISSION_DENIED;
            goto ErrorHandler;
         }

         //value_p = of_calloc(1,sizeof(int32_t)+1);
         //Considering Boundary conditions for copying integer to char buffer
         //Int -2^31 to +2^31, +1 for sign and +1 for null character = 12
         value_p = of_calloc(1,12);
         if(!value_p)
         {
            CM_JE_DEBUG_PRINT ("Memory allocation failed");
            return OF_FAILURE;
         }
         sprintf(value_p,"%d",iValNum);
         of_free(pNvPair->value_p);
         pNvPair->value_p = value_p;

         break;
      case CM_DATA_TYPE_UINT:
         if (!ucmJEIsDigits ((char *) value_p, uiValLen))
         {
            error_code = UCMJE_DATA_ELEM_VALTYPE_INVALID_UINT;
            goto ErrorHandler;
         }
         uiValNum = of_atoi (value_p);
         if( (bNonEdit == TRUE) &&
               (uiValNum != attrib_p->data_attrib.default_value.uidefault_value) )
         {
            CM_JE_DEBUG_PRINT ("Key does not have permissions to edit");
            error_code = UCMJE_ERROR_PERMISSION_DENIED;
            goto ErrorHandler;
         }

         //value_p = of_calloc(1,sizeof(uint32_t)+1);
         //Considering Boundary conditions for copying integer to char buffer
         //Int 0 to +2^32 and +1 for null character = 11
         value_p = of_calloc(1,11);
         if(!value_p)
         {
            CM_JE_DEBUG_PRINT ("Memory allocation failed");
            return OF_FAILURE;
         }
         sprintf(value_p,"%ld",uiValNum);
         of_free(pNvPair->value_p);
         pNvPair->value_p = value_p;
         break;

      case CM_DATA_TYPE_INT64:
         if (!ucmJEIsDigits ((char *) value_p, uiValLen))
         {
            error_code = UCMJE_DATA_ELEM_VALTYPE_INVALID_INT64;
            goto ErrorHandler;
         }
         iValNum = of_atol (value_p);
         if( (bNonEdit == TRUE) &&
               (iValNum != attrib_p->data_attrib.default_value.ldefault_value) )
         {
            CM_JE_DEBUG_PRINT ("Key does not have permissions to edit");
            error_code = UCMJE_ERROR_PERMISSION_DENIED;
            goto ErrorHandler;
         }

         //value_p = of_calloc(1,sizeof(int32_t)+1);
         //Considering Boundary conditions for copying integer to char buffer
         //Int -2^63 to +2^63, +1 for sign and +1 for null character = 22
         value_p = of_calloc(1,22);
         if(!value_p)
         {
            CM_JE_DEBUG_PRINT ("Memory allocation failed");
            return OF_FAILURE;
         }
         sprintf(value_p,"%ld",iValNum);
         of_free(pNvPair->value_p);
         pNvPair->value_p = value_p;

         break;
      case CM_DATA_TYPE_UINT64:
        /* if (!ucmJEIsDigits ((char *) value_p, uiValLen))
         {
            error_code = UCMJE_DATA_ELEM_VALTYPE_INVALID_UINT64;
            goto ErrorHandler;
         }*/
         uiValNum = of_atol (value_p);
         if( (bNonEdit == TRUE) &&
               (uiValNum != attrib_p->data_attrib.default_value.uldefault_value) )
         {
            CM_JE_DEBUG_PRINT ("Key does not have permissions to edit");
            error_code = UCMJE_ERROR_PERMISSION_DENIED;
            goto ErrorHandler;
         }

         //value_p = of_calloc(1,sizeof(uint32_t)+1);
         //Considering Boundary conditions for copying integer to char buffer
         //Int 0 to +2^64 and +1 for null character = 21
       /*  value_p = of_calloc(1,21);
         if(!value_p)
         {
            CM_JE_DEBUG_PRINT ("Memory allocation failed");
            return OF_FAILURE;
         }
         sprintf(value_p,"%ld",uiValNum);
         of_free(pNvPair->value_p);
         pNvPair->value_p = value_p;*/
         break;

      case CM_DATA_TYPE_STRING:
         if( (bNonEdit == TRUE) &&
               (of_strcmp(value_p, attrib_p->data_attrib.default_value.default_string) != 0) )
         {
            CM_JE_DEBUG_PRINT ("Key does not have permissions to edit");
            error_code = UCMJE_ERROR_PERMISSION_DENIED;
            goto ErrorHandler;
         }
         break;
      case CM_DATA_TYPE_DATETIME:
         error_code = UCMJE_DATA_ELEM_VALTYPE_INVALID_DATETIME;
         goto ErrorHandler;
      case CM_DATA_TYPE_IPADDR:
         if (cm_val_and_conv_aton (value_p, &ulIp) != OF_SUCCESS)
         {
            error_code = UCMJE_DATA_ELEM_VALTYPE_INVALID_IPADDR;
            goto ErrorHandler;
         }
         if( (bNonEdit == TRUE) &&
               (of_strcmp(value_p, attrib_p->data_attrib.default_value.default_string) != 0) )
         {
            CM_JE_DEBUG_PRINT ("Key does not have permissions to edit");
            error_code = UCMJE_ERROR_PERMISSION_DENIED;
            goto ErrorHandler;
         }
         break;
#ifdef CM_IPV6_SUPPORT

      case CM_DATA_TYPE_IPV6ADDR:
         if(cm_ipv6_presentation_to_num(value_p, &ulIpv6)!= OF_SUCCESS)
         {
            error_code = UCMJE_DATA_ELEM_VALTYPE_INVALID_IPV6ADDR;
            goto ErrorHandler;
         }
         if( (bNonEdit == TRUE) &&
               (of_strcmp(value_p, attrib_p->data_attrib.default_value.default_string) != 0) )
         {
            CM_JE_DEBUG_PRINT ("Key does not have permissions to edit");
            error_code = UCMJE_ERROR_PERMISSION_DENIED;
            goto ErrorHandler;
         }
         break;

#endif /*CM_IPV6_SUPPORT*/

      case CM_DATA_TYPE_UNKNOWN:
         break;
      case CM_DATA_TYPE_BOOLEAN:
         iValNum = of_atoi (value_p);
         if (!((iValNum == TRUE) || (iValNum == FALSE)))
         {
            error_code = UCMJE_DATA_ELEM_VALTYPE_INVALID_BOOLEAN;
            goto ErrorHandler;
         }
         if( (bNonEdit == TRUE) &&
               (iValNum != attrib_p->data_attrib.default_value.idefault_value) )
         {
            CM_JE_DEBUG_PRINT ("Key does not have permissions to edit");
            error_code = UCMJE_ERROR_PERMISSION_DENIED;
            goto ErrorHandler;
         }
         break;
      case CM_DATA_TYPE_EXCLUDING_XMLSTRINGS:
         if (validate_string_with_out_xml_chars(value_p, of_strlen(value_p)) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Invalid String XML chars are not allowed");         
            error_code = UCMJE_DATA_ELEM_VALTYPE_INVALID_XMLSTRING;
            goto ErrorHandler;
         }
         break;
      case CM_DATA_TYPE_STRING_SPECIAL_CHARS:
         if( (bNonEdit == TRUE) &&
               (of_strcmp(value_p, attrib_p->data_attrib.default_value.default_string) != 0) )
         {
            CM_JE_DEBUG_PRINT ("Key does not have permissions to edit");
            error_code = UCMJE_ERROR_PERMISSION_DENIED;
            goto ErrorHandler;
         }
         break;
      case CM_DATA_TYPE_STRING_PASSWORD:
         break;
      default:
         CM_JE_DEBUG_PRINT ("Invalid Value typere");
         error_code = UCMJE_DATA_ELEM_VALTYPE_INVALID;
         goto ErrorHandler;
   }
   switch (attrib_p->data_attrib.attrib_type)
   {
      case CM_DATA_ATTRIB_INT_RANGE:
         if ((iValNum < attrib_p->data_attrib.attr.int_range.start_value) ||
               (iValNum > attrib_p->data_attrib.attr.int_range.end_value))
         {
            error_code = UCMJE_DATA_ATTRIB_INVALID_INT_RANGE;
            uiResultLen = CM_JE_ERROR_STRING_LEN;
            result_p->result.error.result_string_p =
               (char *) of_calloc (1, uiResultLen + 1);
            if (!result_p->result.error.result_string_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocatin  Failed");
               return OF_FAILURE;
            }
            sprintf (result_p->result.error.result_string_p,
                  "Element Value must be in between %d and %d",
                  attrib_p->data_attrib.attr.int_range.start_value,
                  attrib_p->data_attrib.attr.int_range.end_value);
            goto ErrorHandler;
         }
         break;
      case CM_DATA_ATTRIB_UINT_RANGE:
         if ((uiValNum < attrib_p->data_attrib.attr.uint_range.start_value) ||
               (uiValNum > attrib_p->data_attrib.attr.uint_range.end_value))
         {
            error_code = UCMJE_DATA_ATTRIB_INVALID_UINT_RANGE;
            uiResultLen = CM_JE_ERROR_STRING_LEN;
            result_p->result.error.result_string_p =
               (char *) of_calloc (1, uiResultLen + 1);
            if (!result_p->result.error.result_string_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocatin  Failed");
               return OF_FAILURE;
            }
            sprintf (result_p->result.error.result_string_p,
                  "Element Value must be in between %ld and %ld",
                  attrib_p->data_attrib.attr.uint_range.start_value,
                  attrib_p->data_attrib.attr.uint_range.end_value);
            goto ErrorHandler;
         }
         break;
      case CM_DATA_ATTRIB_INT_ENUM:
         for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.int_enum.count_ui;
               uiCnt++)
         {
            if (iValNum == attrib_p->data_attrib.attr.int_enum.array[uiCnt])
            {
               bMatchFound = TRUE;
            }
         }
         if (bMatchFound == FALSE)
         {
            error_code = UCMJE_DATA_ATTRIB_INVALID_INT_ENUM;
            uiResultLen = (sizeof(int32_t)+2)*attrib_p->data_attrib.attr.int_enum.count_ui;
            //uiResultLen = CM_JE_ERROR_STRING_LEN;
            result_p->result.error.result_string_p =
               (char *) of_calloc (1, uiResultLen + 100);
            if (!result_p->result.error.result_string_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocatin  Failed");
               return OF_FAILURE;
            }
            sprintf (result_p->result.error.result_string_p,
                  "Element Value must be one of the following ");
            for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.int_enum.count_ui;
                  uiCnt++)
            {
               of_memset (aTmpBuf, 0, sizeof (aTmpBuf));
               sprintf (aTmpBuf, "%d, ",
                     attrib_p->data_attrib.attr.int_enum.array[uiCnt]);
               of_strncat (result_p->result.error.result_string_p, aTmpBuf,
                     of_strlen (aTmpBuf));
            }
            goto ErrorHandler;
         }
         break;
      case CM_DATA_ATTRIB_UINT_ENUM:
         for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.uint_enum.count_ui;
               uiCnt++)
         {
            if (uiValNum == attrib_p->data_attrib.attr.uint_enum.array[uiCnt])
            {
               bMatchFound = TRUE;
            }
         }
         if (bMatchFound == FALSE)
         {
            error_code = UCMJE_DATA_ATTRIB_INVALID_UINT_ENUM;
            uiResultLen = (sizeof(uint32_t)+2)*attrib_p->data_attrib.attr.uint_enum.count_ui;
            //uiResultLen = CM_JE_ERROR_STRING_LEN;
            result_p->result.error.result_string_p =
               (char *) of_calloc (1, uiResultLen + 100);
            if (!result_p->result.error.result_string_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocatin  Failed");
               return OF_FAILURE;
            }
            sprintf (result_p->result.error.result_string_p,
                  "Element Value must be one of the following ");
            for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.uint_enum.count_ui;
                  uiCnt++)
            {
               of_memset (aTmpBuf, 0, sizeof (aTmpBuf));
               sprintf (aTmpBuf, "%ld, ",
                     attrib_p->data_attrib.attr.uint_enum.array[uiCnt]);
               of_strncat (result_p->result.error.result_string_p, aTmpBuf,
                     of_strlen (aTmpBuf));
            }
            goto ErrorHandler;
         }
         break;
      case CM_DATA_ATTRIB_INT64_RANGE:
         if ((iValNum < attrib_p->data_attrib.attr.longint_range.start_value) ||
               (iValNum > attrib_p->data_attrib.attr.longint_range.end_value))
         {
            error_code = UCMJE_DATA_ATTRIB_INVALID_INT64_RANGE;
            uiResultLen = CM_JE_ERROR_STRING_LEN;
            result_p->result.error.result_string_p =
               (char *) of_calloc (1, uiResultLen + 1);
            if (!result_p->result.error.result_string_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocatin  Failed");
               return OF_FAILURE;
            }
            sprintf (result_p->result.error.result_string_p,
                  "Element Value must be in between %d and %d",
                  attrib_p->data_attrib.attr.longint_range.start_value,
                  attrib_p->data_attrib.attr.longint_range.end_value);
            goto ErrorHandler;
         }
         break;
      case CM_DATA_ATTRIB_UINT64_RANGE:
         if ((uiValNum < attrib_p->data_attrib.attr.ulongint_range.start_value) ||
               (uiValNum > attrib_p->data_attrib.attr.ulongint_range.end_value))
         {
            error_code = UCMJE_DATA_ATTRIB_INVALID_UINT64_RANGE;
            uiResultLen = CM_JE_ERROR_STRING_LEN;
            result_p->result.error.result_string_p =
               (char *) of_calloc (1, uiResultLen + 1);
            if (!result_p->result.error.result_string_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocatin  Failed");
               return OF_FAILURE;
            }
            sprintf (result_p->result.error.result_string_p,
                  "Element Value must be in between %ld and %ld",
                  attrib_p->data_attrib.attr.ulongint_range.start_value,
                  attrib_p->data_attrib.attr.ulongint_range.end_value);
            goto ErrorHandler;
         }
         break;
      case CM_DATA_ATTRIB_INT64_ENUM:
         for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.longint_enum.count_ui;
               uiCnt++)
         {
            if (iValNum == attrib_p->data_attrib.attr.longint_enum.array[uiCnt])
            {
               bMatchFound = TRUE;
            }
         }
         if (bMatchFound == FALSE)
         {
            error_code = UCMJE_DATA_ATTRIB_INVALID_INT64_ENUM;
            uiResultLen = (sizeof(int32_t)+2)*attrib_p->data_attrib.attr.longint_enum.count_ui;
            //uiResultLen = CM_JE_ERROR_STRING_LEN;
            result_p->result.error.result_string_p =
               (char *) of_calloc (1, uiResultLen + 100);
            if (!result_p->result.error.result_string_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocatin  Failed");
               return OF_FAILURE;
            }
            sprintf (result_p->result.error.result_string_p,
                  "Element Value must be one of the following ");
            for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.longint_enum.count_ui;
                  uiCnt++)
            {
               of_memset (aTmpBuf, 0, sizeof (aTmpBuf));
               sprintf (aTmpBuf, "%ld, ",
                     attrib_p->data_attrib.attr.longint_enum.array[uiCnt]);
               of_strncat (result_p->result.error.result_string_p, aTmpBuf,
                     of_strlen (aTmpBuf));
            }
            goto ErrorHandler;
         }
         break;
      case CM_DATA_ATTRIB_UINT64_ENUM:
         for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.ulongint_enum.count_ui;
               uiCnt++)
         {
            if (uiValNum == attrib_p->data_attrib.attr.ulongint_enum.array[uiCnt])
            {
               bMatchFound = TRUE;
            }
         }
         if (bMatchFound == FALSE)
         {
            error_code = UCMJE_DATA_ATTRIB_INVALID_UINT64_ENUM;
            uiResultLen = (sizeof(uint32_t)+2)*attrib_p->data_attrib.attr.ulongint_enum.count_ui;
            //uiResultLen = CM_JE_ERROR_STRING_LEN;
            result_p->result.error.result_string_p =
               (char *) of_calloc (1, uiResultLen + 100);
            if (!result_p->result.error.result_string_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocatin  Failed");
               return OF_FAILURE;
            }
            sprintf (result_p->result.error.result_string_p,
                  "Element Value must be one of the following ");
            for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.ulongint_enum.count_ui;
                  uiCnt++)
            {
               of_memset (aTmpBuf, 0, sizeof (aTmpBuf));
               sprintf (aTmpBuf, "%ld, ",
                     attrib_p->data_attrib.attr.ulongint_enum.array[uiCnt]);
               of_strncat (result_p->result.error.result_string_p, aTmpBuf,
                     of_strlen (aTmpBuf));
            }
            goto ErrorHandler;
         }
         break;

      case CM_DATA_ATTRIB_STR_ENUM:
         for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.string_enum.count_ui;
               uiCnt++)
         {
            if (!of_strcmp
                  (value_p, attrib_p->data_attrib.attr.string_enum.array[uiCnt]))
            {
               bMatchFound = TRUE;
               break;
            }
         }
         if (bMatchFound == FALSE)
         {
            error_code = UCMJE_DATA_ATTRIB_INVALID_STR_ENUM;
            uiResultLen = 0;
            for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.string_enum.count_ui;
                  uiCnt++)
            {
               uiResultLen += of_strlen(attrib_p->data_attrib.attr.string_enum.array[uiCnt]) +2;
            }
            //uiResultLen = CM_JE_ERROR_STRING_LEN;
            result_p->result.error.result_string_p =
               (char *) of_calloc (1, uiResultLen + 100);
            if (!result_p->result.error.result_string_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocatin  Failed");
               return OF_FAILURE;
            }
            sprintf (result_p->result.error.result_string_p,
                  "Element Value must be one of the following ");
            for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.string_enum.count_ui;
                  uiCnt++)
            {
               of_memset (aTmpBuf, 0, sizeof (aTmpBuf));
               sprintf (aTmpBuf, "%s, ",
                     attrib_p->data_attrib.attr.string_enum.array[uiCnt]);
               of_strncat (result_p->result.error.result_string_p, aTmpBuf,
                     of_strlen (aTmpBuf));
            }
            goto ErrorHandler;
         }
         break;
      case CM_DATA_ATTRIB_STR_MULTVAL:
         pToken = strtok(value_p,","); 
         while(pToken != NULL)
         {
            bMatchFound = FALSE;
            /* printf("Token %s \n",pToken); */
            for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.string_enum.count_ui;
                  uiCnt++)
            { 
               if (!of_strcmp
                     (pToken, attrib_p->data_attrib.attr.string_enum.array[uiCnt]))
               {
                  bMatchFound = TRUE;
                  break;
               }
            }

            if (bMatchFound == FALSE)
            {
               error_code = UCMJE_DATA_ATTRIB_INVALID_STR_ENUM;
               uiResultLen = 0;
               for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.string_enum.count_ui;
                     uiCnt++)
               {
                  uiResultLen += of_strlen(attrib_p->data_attrib.attr.string_enum.array[uiCnt]) +2;
               }
               //uiResultLen = CM_JE_ERROR_STRING_LEN;
               result_p->result.error.result_string_p =
                  (char *) of_calloc (1, uiResultLen + 100);
               if (!result_p->result.error.result_string_p)
               {
                  CM_JE_DEBUG_PRINT ("Memory Allocatin  Failed");
                  return OF_FAILURE;
               }
               sprintf (result_p->result.error.result_string_p,
                     "Element Value must be one of the following ");
               for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.string_enum.count_ui;
                     uiCnt++)
               {
                  of_memset (aTmpBuf, 0, sizeof (aTmpBuf));
                  sprintf (aTmpBuf, "%s, ",
                        attrib_p->data_attrib.attr.string_enum.array[uiCnt]);
                  of_strncat (result_p->result.error.result_string_p, aTmpBuf,
                        of_strlen (aTmpBuf));
               }
               goto ErrorHandler;
            } 
            pToken = strtok(NULL,",");
         }

         break;
      case CM_DATA_ATTRIB_STR_MULTVAL_NOVALD:
         break;
      case CM_DATA_ATTRIB_STR_RANGE:
         uiStrLen = of_strlen (value_p);
         if ((uiStrLen < attrib_p->data_attrib.attr.string_range.min_length) ||
               (uiStrLen > attrib_p->data_attrib.attr.string_range.max_length))
         {
            error_code = UCMJE_DATA_ATTRIB_INVALID_STR_RANGE;
            uiResultLen = CM_JE_ERROR_STRING_LEN;
            result_p->result.error.result_string_p =
               (char *) of_calloc (1, uiResultLen + 1);
            if (!result_p->result.error.result_string_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocatin  Failed");
               return OF_FAILURE;
            }
            sprintf (result_p->result.error.result_string_p,
                  "Element Value Length must be in between %ld and %ld",
                  attrib_p->data_attrib.attr.string_range.min_length,
                  attrib_p->data_attrib.attr.string_range.max_length);
            goto ErrorHandler;
         }
         break;
      case CM_DATA_ATTRIB_NONE:
         CM_JE_DEBUG_PRINT ("Attribute is None: Ignoring");
         break;
      case CM_DATA_ATTRIB_IPADDR_ENUM:
         for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.ip_enum.count_ui; uiCnt++)
         {
            if (!strcmp (value_p, attrib_p->data_attrib.attr.ip_enum.ip_arr_a[uiCnt]))
            {
               bMatchFound = TRUE;
            }
         }
         if (bMatchFound == FALSE)
         {
            error_code = UCMJE_DATA_ATTRIB_INVALID_IPADDR_ENUM;
            uiResultLen = 0;
            for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.ip_enum.count_ui;
                  uiCnt++)
            {
               uiResultLen += of_strlen(attrib_p->data_attrib.attr.ip_enum.ip_arr_a[uiCnt]) +2;
            }
            //uiResultLen = CM_JE_ERROR_STRING_LEN;
            result_p->result.error.result_string_p =
               (char *) of_calloc (1, uiResultLen + 100);
            if (!result_p->result.error.result_string_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocatin  Failed");
               return OF_FAILURE;
            }
            sprintf (result_p->result.error.result_string_p,
                  "Element Value must be one of the following ");
            for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.ip_enum.count_ui;
                  uiCnt++)
            {
               of_memset (aTmpBuf, 0, sizeof (aTmpBuf));
               sprintf (aTmpBuf, "%s, ",
                     attrib_p->data_attrib.attr.ip_enum.ip_arr_a[uiCnt]);
               of_strncat (result_p->result.error.result_string_p, aTmpBuf,
                     of_strlen (aTmpBuf));
            }
            goto ErrorHandler;
         }
         break;
      case CM_DATA_ATTRIB_IPADDR_RANGE:
         if (cm_val_and_conv_aton
               (attrib_p->data_attrib.attr.ip_range.start_ip, &ulStartIp) != OF_SUCCESS)
         {
            error_code = UCMJE_DATA_ELEM_VALTYPE_INVALID_IPADDR;
            goto ErrorHandler;
         }
         if (cm_val_and_conv_aton
               (attrib_p->data_attrib.attr.ip_range.end_ip, &ulEndIp) != OF_SUCCESS)
         {
            error_code = UCMJE_DATA_ELEM_VALTYPE_INVALID_IPADDR;
            goto ErrorHandler;
         }
         if ((ulIp < ulStartIp) || (ulIp > ulEndIp))
         {
            error_code = UCMJE_DATA_ATTRIB_INVALID_IPADDR_RANGE;
            uiResultLen = CM_JE_ERROR_STRING_LEN;
            result_p->result.error.result_string_p =
               (char *) of_calloc (1, uiResultLen + 1);
            if (!result_p->result.error.result_string_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocatin  Failed");
               return OF_FAILURE;
            }
            sprintf (result_p->result.error.result_string_p,
                  "Element Value Length must be in between %s and %s",
                  attrib_p->data_attrib.attr.ip_range.start_ip,
                  attrib_p->data_attrib.attr.ip_range.end_ip);
            goto ErrorHandler;
         }
         break;
#ifdef CM_IPV6_SUPPORT

      case CM_DATA_ATTRIB_IPV6ADDR_ENUM:
         for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.ipv6_enum.count_ui; uiCnt++)
         {    
            if (!strcmp (value_p, attrib_p->data_attrib.attr.ipv6_enum.ip_v6_arr_a[uiCnt]))
            {
               bMatchFound = TRUE;
            }
         }    
         if (bMatchFound == FALSE)
         {    
            error_code = UCMJE_DATA_ATTRIB_INVALID_IPADDR_ENUM;
            uiResultLen = 0;
            for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.ipv6_enum.count_ui;
                  uiCnt++)
            {
               uiResultLen += of_strlen(attrib_p->data_attrib.attr.ipv6_enum.ip_v6_arr_a[uiCnt]) +2;
            }
            //uiResultLen = CM_JE_ERROR_STRING_LEN;
            result_p->result.error.result_string_p =
               (char *) of_calloc (1, uiResultLen + 100);
            if (!result_p->result.error.result_string_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocatin  Failed");
               return OF_FAILURE;
            }
            sprintf (result_p->result.error.result_string_p,
                  "Element Value must be one of the following ");
            for (uiCnt = 0; uiCnt < attrib_p->data_attrib.attr.ipv6_enum.count_ui;
                  uiCnt++)
            {
               of_memset (aTmpBuf, 0, sizeof (aTmpBuf));
               sprintf (aTmpBuf, "%s, ",
                     attrib_p->data_attrib.attr.ipv6_enum.ip_v6_arr_a[uiCnt]);
               of_strncat (result_p->result.error.result_string_p, aTmpBuf,
                     of_strlen (aTmpBuf));
            }
            goto ErrorHandler;
         }    
         break;

      case CM_DATA_ATTRIB_IPV6ADDR_RANGE:
         if (cm_ipv6_presentation_to_num
               (attrib_p->data_attrib.attr.ipv6_range.start_ip, &ulStartIpv6) != OF_SUCCESS)
         {
            error_code = UCMJE_DATA_ELEM_VALTYPE_INVALID_IPADDR;
            goto ErrorHandler;
         }
         if (cm_ipv6_presentation_to_num
               (attrib_p->data_attrib.attr.ipv6_range.end_ip, &ulEndIpv6) != OF_SUCCESS)
         {
            error_code = UCMJE_DATA_ELEM_VALTYPE_INVALID_IPADDR;
            goto ErrorHandler;
         }
         for(ii=0; ii< CM_IPV6_ADDR_LEN ; ii++)
         {
            if(ulIpv6.s6_addr[ii] < ulStartIpv6.s6_addr[ii] && bStart == FALSE) 
            { 
               bStart = TRUE; 
               if(bEnd == TRUE)
                  break;
            }

            if(ulIpv6.s6_addr[ii] > ulStartIpv6.s6_addr[ii] && bEnd == FALSE)
            {
               bEnd = TRUE;
               if(bStart == TRUE)
                  break;
            }
         }
         if (bStart == FALSE || bEnd == FALSE)
         {
            error_code = UCMJE_DATA_ATTRIB_INVALID_IPADDR_RANGE;
            uiResultLen = CM_JE_ERROR_STRING_LEN;
            result_p->result.error.result_string_p =
               (char *) of_calloc (1, uiResultLen + 1);
            if (!result_p->result.error.result_string_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocatin  Failed");
               return OF_FAILURE;
            }
            sprintf (result_p->result.error.result_string_p,
                  "Element Value Length must be in between %s and %s",
                  attrib_p->data_attrib.attr.ipv6_range.start_ip,
                  attrib_p->data_attrib.attr.ipv6_range.end_ip);
            goto ErrorHandler;
         }
         break;

#endif /*CM_IPV6_SUPPORT*/

      default:
         CM_JE_DEBUG_PRINT ("Unknonwn  type: default");
         break;
   }

   return OF_SUCCESS;

ErrorHandler:
   CM_JE_DEBUG_PRINT ("Error Code=%u and Error Nv Pair...", error_code);
   result_p->result.error.error_code = error_code;
   if (error_code != UCMJE_DATA_ELEM_VALUE_NULL)
   {
      result_p->result.error.nv_pair_b = TRUE;
      UCMJE_COPY_PTR_NVPAIR_TO_NVPAIR (pNvPair,
            result_p->result.error.nv_pair);
      CM_JE_PRINT_NVPAIR (result_p->result.error.nv_pair);
   }
   return OF_FAILURE;
}

/**
  \ingroup JEAPI
  This API Checks for all mandatory parameters recieved or not for Data Model\n
  Table Nodes in ADD configuration Cache.
  <b>Input paramameters: </b>\n
  <b><i>config_session_p:</i></b> Pointer to Configuratin Session
  <b>Output Parameters: </b>None  \n
  <b><i>result_p</i></b> Pointer to UCMResult which contains failure information
  in failure case.\n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_check_session_mandatory_params 
 ** Description   : This API Checks for all the mandatory parameters for all Data
 **                  Model Table Nodes to be added in configuration session.
 ** Input params  : config_session_p  - Pointer to Configuratin Session
 ** Output params : result_p - Pointer to UCMResult 
 **                      In Failure case it contains error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t je_check_session_mandatory_params (struct je_config_session *
      config_session_p, struct cm_result * result_p)
{
   struct je_command_list *command_list_p = NULL;

   uint32_t return_value;
   UCMDllQNode_t *pDllQNode;

   if (config_session_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("config_session_p is NULL");
      return OF_FAILURE;
   }
   if (config_session_p->pCache == NULL)
   {
      CM_JE_DEBUG_PRINT ("Configuration Cache is not initialized");
      return OF_FAILURE;
   }

   CM_DLLQ_SCAN (&(config_session_p->pCache->add_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      if (command_list_p && command_list_p->command_p)
      {
         if((return_value = je_check_mandatory_params(command_list_p, result_p)) != OF_SUCCESS)
         {
            return OF_FAILURE;
         }
      }
   }

   CM_DLLQ_SCAN (&(config_session_p->pCache->xtn_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      if (command_list_p && command_list_p->command_p)
      {
         if(command_list_p->command_p->command_id == CM_CMD_ADD_PARAMS)
         {
            if((return_value = je_check_mandatory_params(command_list_p, result_p)) != OF_SUCCESS)
            {
               return OF_FAILURE;
            }
         }
      }
   }

   return OF_SUCCESS;
}

 int32_t je_check_mandatory_params(struct je_command_list * command_list_p, struct cm_result * result_p)
{
   struct cm_dm_data_element *pDMNode = NULL;
   struct cm_dm_data_element *pChildNode = NULL;

   uint32_t uiNvCnt;
   UCMDllQ_t *pDll;
   UCMDllQNode_t *pNode;

   char *dm_path_p = command_list_p->command_p->dm_path_p;

   if (!dm_path_p || of_strlen(dm_path_p) == 0)
   {
      CM_JE_DEBUG_PRINT ("DM Path is NULL");
      result_p->result.error.error_code = UCMJE_ERROR_DMPATH_NULL;
      return OF_FAILURE;
   }

   pDMNode = (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path
      (dm_path_p, of_strlen (dm_path_p));
   if (!pDMNode)
   {
      CM_JE_DEBUG_PRINT ("pDMNode is NULL");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_GETNODE_BYDMPATH,
            dm_path_p);
      return OF_FAILURE;
   }

   pDll = &(pDMNode->child_list);
   CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
   {
      pChildNode =
         CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
      if (pChildNode->element_attrib.mandatory_b == TRUE)
      {
         struct cm_array_of_nv_pair *pNvPairAry = &command_list_p->command_p->nv_pair_array;
         unsigned char bNameFound = FALSE;

         for (uiNvCnt = 0; uiNvCnt < pNvPairAry->count_ui; uiNvCnt++)
         {
            if (!of_strcmp(pChildNode->name_p, pNvPairAry->nv_pairs[uiNvCnt].name_p))
            {
               bNameFound = TRUE;
            }
         }
         if (bNameFound == FALSE)
         {
            if (pChildNode->element_attrib.visible_b == FALSE)
            {
               CM_JE_DEBUG_PRINT ("%s not a configurable parameter",
                     pChildNode->name_p);
               continue;
            }
            CM_JE_DEBUG_PRINT ("%s not set", pChildNode->name_p);
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_MANDATORY_PARAM_NOT_SET,
                  dm_path_p);
            UCMJE_MEMALLOC_AND_COPY_RESULTSTRING_INTO_UCMRESULT (result_p,
                  pChildNode->name_p);
            return OF_FAILURE;
         }
      }
   }
   return OF_SUCCESS;
}



/**
  \ingroup JEAPI
  This API Executes Test Function for each command in Configuration Session
  <b>Input paramameters: </b>\n
  <b><i>config_session_p:</i></b> Pointer to Configuration Session
  <b>Output Parameters: </b>
  <b><i>result_p</i></b> double pointer to UCMResult
  In Failure case error information and in success case none.  
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_execute_test_func_handler 
 ** Description   : This API Executes Test functions for each command in Configuration
 Session
 ** Input params  : config_session_p  - Pointer to Configuration Session
 ** Output params : result_p - double pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t je_execute_test_func_handler (struct je_config_session *
      config_session_p, struct cm_result * result_p)
{
   struct je_command_list *command_list_p = NULL;
   UCMDllQNode_t *pDllQNode = NULL;
   struct cm_dm_data_element *pDMNode = NULL;
   char *dm_path_p=NULL;
   uint32_t mgmt_engine_id=0;


   if (config_session_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("config_session_p is NULL");
      return OF_FAILURE;
   }
   if (config_session_p->pCache == NULL)
   {
      CM_JE_DEBUG_PRINT ("Configuration Cache is not initialized");
      return OF_FAILURE;
   }

   mgmt_engine_id = config_session_p->mgmt_engine_id;

   CM_DLLQ_SCAN (&(config_session_p->pCache->add_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      if (command_list_p && command_list_p->command_p)
      {
         /*** if its a command in the transaction then we can ignore it while handling commit ***/
         dm_path_p = command_list_p->command_p->dm_path_p;

         pDMNode = (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path
            (dm_path_p, of_strlen (dm_path_p));
         if (!pDMNode)
         {
            CM_JE_DEBUG_PRINT ("pDMNode is NULL");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_GETNODE_BYDMPATH,
                  dm_path_p);
            return OF_FAILURE;
         }

         if (je_execute_command_test_func (command_list_p->command_p->dm_path_p,
                  &command_list_p->command_p->nv_pair_array,
                  CM_CMD_ADD_PARAMS,
                  command_list_p->referrence_in_cache_b,
                  result_p,
                  mgmt_engine_id) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Test Function Failed");
            return OF_FAILURE;
         }
      }
   }

   CM_DLLQ_SCAN (&(config_session_p->pCache->modify_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      if (command_list_p && command_list_p->command_p)
      {
         dm_path_p = command_list_p->command_p->dm_path_p;

         pDMNode = (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path
            (dm_path_p, of_strlen (dm_path_p));
         if (!pDMNode)
         {
            CM_JE_DEBUG_PRINT ("pDMNode is NULL");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_GETNODE_BYDMPATH,
                  dm_path_p);
            return OF_FAILURE;
         }

         if (je_execute_command_test_func (command_list_p->command_p->dm_path_p,
                  &command_list_p->command_p->nv_pair_array,
                  CM_CMD_SET_PARAMS,
                  command_list_p->referrence_in_cache_b,
                  result_p,
                  mgmt_engine_id) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Test Function Failed");
            return OF_FAILURE;
         }
      }
   }

   CM_DLLQ_SCAN (&(config_session_p->pCache->delete_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      if (command_list_p && command_list_p->command_p)
      {
         dm_path_p = command_list_p->command_p->dm_path_p;

         pDMNode = (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path
            (dm_path_p, of_strlen (dm_path_p));
         if (!pDMNode)
         {
            CM_JE_DEBUG_PRINT ("pDMNode is NULL");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_GETNODE_BYDMPATH,
                  dm_path_p);
            return OF_FAILURE;
         }

         if (je_execute_command_test_func (command_list_p->command_p->dm_path_p,
                  &command_list_p->command_p->nv_pair_array,
                  CM_CMD_DEL_PARAMS,
                  command_list_p->referrence_in_cache_b,
                  result_p,
                  mgmt_engine_id) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Test Function Failed");
            return OF_FAILURE;
         }
      }
   }
   CM_DLLQ_SCAN (&(config_session_p->pCache->xtn_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      if (command_list_p && command_list_p->command_p)
      {
         dm_path_p = command_list_p->command_p->dm_path_p;

         pDMNode = (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path
            (dm_path_p, of_strlen (dm_path_p));
         if (!pDMNode)
         {
            CM_JE_DEBUG_PRINT ("pDMNode is NULL");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_GETNODE_BYDMPATH,
                  dm_path_p);
            return OF_FAILURE;
         }

         if (je_execute_command_test_func (command_list_p->command_p->dm_path_p,
                  &command_list_p->command_p->nv_pair_array,
                  command_list_p->command_p->command_id,
                  command_list_p->referrence_in_cache_b,
                  result_p,
                  mgmt_engine_id) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Test Function Failed");
            return OF_FAILURE;
         }
      }
   }
   return OF_SUCCESS;
}

/**
  \ingroup JEAPI
  This API Executes Registered Callback Functions for each command's Data Model Path
  in Configuration Session
  <b>Input paramameters: </b>\n
  <b><i>config_session_p:</i></b> Pointer to Configuration Session
  <b>Output Parameters: </b>
  <b><i>result_p</i></b> double pointer to UCMResult
  In Failure case error information and in success case none.  
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name :je_execute_cbk_func_handler 
 ** Description   : This API Executes Registered Callback functions for each command
 in Configuration  Session
 ** Input params  : config_session_p  - Pointer to Configuration Session
 ** Output params : result_p - double pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t je_execute_cbk_func_handler (struct je_config_session *
      config_session_p,
      uint16_t * pusCbkSuccessCnt,
      struct cm_result * result_p)
{
   struct je_command_list *command_list_p = NULL;
   int32_t return_value = 0;
   uint32_t uiPathLen = 0;
   char *parent_path_p=NULL;
   UCMDllQNode_t *pDllQNode = NULL;
   struct cm_array_of_nv_pair key_nv_pairs={};
   char *dm_path_p;
   struct cm_dm_data_element *pDMNode;
   uint32_t mgmt_engine_id=0;



   if (config_session_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("config_session_p is NULL");
      return OF_FAILURE;
   }
   if (config_session_p->pCache == NULL)
   {
      CM_JE_DEBUG_PRINT ("Configuration Cache is not initialized");
      return OF_FAILURE;
   }

   mgmt_engine_id = config_session_p->mgmt_engine_id;

   CM_DLLQ_SCAN (&(config_session_p->pCache->add_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      if (command_list_p && command_list_p->command_p)
      {
         dm_path_p = command_list_p->command_p->dm_path_p;

         pDMNode = (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path
            (dm_path_p, of_strlen (dm_path_p));
         if (!pDMNode)
         {    
            CM_JE_DEBUG_PRINT ("pDMNode is NULL");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_GETNODE_BYDMPATH,
                  dm_path_p);
            return OF_FAILURE;
         }    

         if ((return_value = je_execute_command_cbk_func (dm_path_p,
                     &command_list_p->command_p->nv_pair_array,
                     CM_CMD_ADD_PARAMS,
                     result_p,
                     EXEC_COMPLETE_CMD,
                     mgmt_engine_id)) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Execute Command Cbk Failed");
#ifdef CM_AUDIT_SUPPORT
            UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_ADD_FAIL,config_session_p->admin_name,config_session_p->admin_role,
                  command_list_p->command_p, dm_path_p, NULL,&command_list_p->command_p->nv_pair_array);
#endif
            return OF_FAILURE;
         }
         command_list_p->success_b = TRUE;
         (*pusCbkSuccessCnt)++;

         if(pDMNode->element_attrib.non_config_leaf_node_b == TRUE_ADD)
         {
            continue;
         }

         if ((return_value = je_get_key_nvpair_arr (dm_path_p,
                     &command_list_p->command_p->nv_pair_array,
                     &key_nv_pairs)) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Get Key NvPair Array Failed");
            return OF_FAILURE;
         }
         uiPathLen = of_strlen(dm_path_p);
         parent_path_p = (char *)of_calloc(1,uiPathLen+1);

         cm_remove_last_node_instance_from_dmpath(dm_path_p,parent_path_p);        

         if ((return_value = je_add_instance_map_entry (parent_path_p, &key_nv_pairs)) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("JE Add Instance Map Entry  failure");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  return_value, dm_path_p);
            je_free_nvpair_array (&key_nv_pairs);
            of_free(parent_path_p);
            return OF_FAILURE;
         }
#ifdef CM_ROLES_PERM_SUPPORT
         if ((return_value = cm_je_set_role_to_instance (parent_path_p,
                     config_session_p->admin_role,
                     &key_nv_pairs)) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("JE Set Role Permissions  failure");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  return_value, dm_path_p);
            je_free_nvpair_array (&key_nv_pairs);
            of_free(parent_path_p); /*CID 348*/        
            return OF_FAILURE;
         }
#endif
         je_free_nvpair_array (&key_nv_pairs);
         of_free(parent_path_p); /*CID 348*/
      }
   }

   CM_DLLQ_SCAN (&(config_session_p->pCache->modify_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      if (command_list_p && command_list_p->command_p)
      {
         dm_path_p = command_list_p->command_p->dm_path_p;
         /*** if its a command in the transaction then we can ignore it while handling commit ***/
         pDMNode = (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path
            (dm_path_p, of_strlen (dm_path_p));
         if (!pDMNode)
         {
            CM_JE_DEBUG_PRINT ("pDMNode is NULL");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_GETNODE_BYDMPATH,
                  dm_path_p);
            return OF_FAILURE;
         }

         if ((return_value = je_execute_command_cbk_func (command_list_p->command_p->dm_path_p,
                     &command_list_p->command_p->nv_pair_array,
                     CM_CMD_SET_PARAMS,
                     result_p,
                     EXEC_COMPLETE_CMD,
                     mgmt_engine_id)) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Execute Command Cbk Failed");
#ifdef CM_AUDIT_SUPPORT
            UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_SET_FAIL,config_session_p->admin_name,config_session_p->admin_role,
                  command_list_p->command_p, dm_path_p, NULL,&command_list_p->command_p->nv_pair_array);
#endif
            return OF_FAILURE;
         }
         command_list_p->success_b = TRUE;
         (*pusCbkSuccessCnt)++;
      }
   }

   CM_DLLQ_SCAN (&(config_session_p->pCache->delete_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      if (command_list_p && command_list_p->command_p)
      {
         dm_path_p = command_list_p->command_p->dm_path_p;

         uiPathLen = of_strlen(dm_path_p);
         parent_path_p = (char *)of_calloc(1,uiPathLen+1);

         cm_remove_last_node_instance_from_dmpath(dm_path_p,parent_path_p);

         pDMNode = (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path
            (dm_path_p, of_strlen (dm_path_p));
         if (!pDMNode)
         {
            CM_JE_DEBUG_PRINT ("pDMNode is NULL");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_GETNODE_BYDMPATH,
                  dm_path_p);
            return OF_FAILURE;
         }

         if (return_value = cmje_delete_subtree (parent_path_p,
                  &(command_list_p->command_p->nv_pair_array),
                  result_p,
                  DEL_RECORD,
                  EXEC_COMPLETE_CMD,
                  mgmt_engine_id) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Delete Subtree failed");
#ifdef CM_AUDIT_SUPPORT
            UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_SET_FAIL,config_session_p->admin_name,config_session_p->admin_role,
                  command_list_p->command_p, dm_path_p, NULL,&command_list_p->command_p->nv_pair_array);
#endif
            of_free(parent_path_p);
            return OF_FAILURE;
         }
         command_list_p->success_b = TRUE;
         (*pusCbkSuccessCnt)++;
         of_free(parent_path_p);
      }
   }

   if(je_execute_trans_commands(config_session_p,pusCbkSuccessCnt,result_p)!= OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Executing transaction commands failed");
      return OF_FAILURE;
   }

   CM_JE_DEBUG_PRINT ("Successfuly Executed");
   return OF_SUCCESS;
}

 int32_t je_execute_trans_commands (struct je_config_session *
      config_session_p,
      uint16_t * pusCbkSuccessCnt,
      struct cm_result * result_p)
{
   struct je_command_list *command_list_p = NULL;
   int32_t return_value = 0;
   uint32_t uiPathLen = 0;
   uint32_t state;
   uint32_t command_id;
   uint32_t mgmt_engine_id=0;
   char *parent_path_p=NULL;
   UCMDllQNode_t *pDllQNode = NULL;
   struct cm_array_of_nv_pair key_nv_pairs={};
   char *dm_path_p;
   struct cm_dm_data_element *pDMNode;
   //  unsigned char global_trans_b=FALSE;
   unsigned char parent_trans_b=FALSE;


   if (config_session_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("config_session_p is NULL");
      return OF_FAILURE;
   }
   if (config_session_p->pCache == NULL)
   {
      CM_JE_DEBUG_PRINT ("Configuration Cache is not initialized");
      return OF_FAILURE;
   }

   mgmt_engine_id = config_session_p->mgmt_engine_id;


   for (state = START_TRANSACTION ; state < COMMAND_EXECUTED ; state++)
   {
      CM_DLLQ_SCAN (&(config_session_p->pCache->xtn_config_list), pDllQNode, UCMDllQNode_t *)
      {
         command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
         if (command_list_p && command_list_p->command_p && command_list_p->xtn_b == TRUE)
         {
            /*
               if(command_list_p->state == COMMAND_EXECUTED)
               {
               continue;
               }
               */

            command_id = command_list_p->command_p->command_id;

            dm_path_p = command_list_p->command_p->dm_path_p;

            pDMNode = (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path
               (dm_path_p, of_strlen (dm_path_p));
            if (!pDMNode)
            {
               CM_JE_DEBUG_PRINT ("pDMNode is NULL");
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                     UCMJE_ERROR_GETNODE_BYDMPATH,
                     dm_path_p);
               return OF_FAILURE;
            }

            if(pDMNode->element_attrib.parent_trans_b == TRUE)
            {
               parent_trans_b = TRUE;
            }

            if(state == START_TRANSACTION && (command_list_p->state >= EXEC_CBK_FUNC))
            {
               CM_JE_DEBUG_PRINT("Transaction already started");
               continue;
            }
            if(state == START_TRANSACTION)
            {
               if(parent_trans_b == TRUE)
               {
                  command_list_p->state = EXEC_CBK_FUNC;
                  continue;
               }
               else if(je_start_transaction(command_list_p,command_id,result_p,mgmt_engine_id)!= OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("starting transaction failed");
                  return OF_FAILURE;
               }
               command_list_p->state = EXEC_CBK_FUNC;
            }

            if(state == EXEC_CBK_FUNC &&  (command_list_p->state >= IS_IT_OK_FUNC))
            {
               CM_JE_DEBUG_PRINT("Command already executed");
               continue;
            }

            if(state == IS_IT_OK_FUNC && command_list_p->state >= END_TRANSACTION)
            {
               CM_JE_DEBUG_PRINT("Already Executed is it ok fucntion");
               continue;
            }

            if((state == EXEC_CBK_FUNC || state == IS_IT_OK_FUNC || state == END_TRANSACTION )&&
                  (command_id == CM_CMD_ADD_PARAMS || command_id == CM_CMD_SET_PARAMS))
            {
               if ((return_value = je_execute_command_cbk_func (dm_path_p,
                           &command_list_p->command_p->nv_pair_array,
                           command_id,
                           result_p,
                           state,
                           mgmt_engine_id)) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Execute Command Cbk Failed");
                  return OF_FAILURE;
               }
               if(state == EXEC_CBK_FUNC)
               {
                  command_list_p->state = IS_IT_OK_FUNC;
               }
               if(state == IS_IT_OK_FUNC)
               {
                  command_list_p->state = END_TRANSACTION ;
               }
               if(state == END_TRANSACTION)
               {
                  command_list_p->state = COMMAND_EXECUTED ;
                  command_list_p->success_b = TRUE;
                  (*pusCbkSuccessCnt)++;
               }
            }

            if((state == EXEC_CBK_FUNC || state == IS_IT_OK_FUNC || state == END_TRANSACTION )&&
                  (command_id == CM_CMD_DEL_PARAMS ))
            {
               uiPathLen = of_strlen(dm_path_p);
               parent_path_p = (char *)of_calloc(1,uiPathLen+1);

               cm_remove_last_node_instance_from_dmpath(dm_path_p,parent_path_p);

               if (return_value = cmje_delete_subtree (parent_path_p,
                        &(command_list_p->command_p->nv_pair_array),
                        result_p,
                        DEL_RECORD,
                        state,
                        mgmt_engine_id) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Delete Subtree failed");
                  of_free(parent_path_p);
                  return OF_FAILURE;
               }

               if(state == EXEC_CBK_FUNC)
               {
                  command_list_p->state = IS_IT_OK_FUNC;
               }
               if(state == IS_IT_OK_FUNC)
               {
                  command_list_p->state = END_TRANSACTION ;
               }
               if(state == END_TRANSACTION)
               {
                  if (return_value = cmje_delete_subtree (parent_path_p,
                           &(command_list_p->command_p->nv_pair_array),
                           result_p,
                           DEL_INSTANCE_ONLY,
                           EXEC_COMPLETE_CMD,
                           mgmt_engine_id) != OF_SUCCESS)
                  {
                     CM_JE_DEBUG_PRINT ("Delete Subtree failed");
                     of_free(parent_path_p);
                     return OF_FAILURE;
                  }
                  command_list_p->state = COMMAND_EXECUTED ;
                  command_list_p->success_b = TRUE;
                  (*pusCbkSuccessCnt)++;
               }
               of_free(parent_path_p);
            }

            if(state == END_TRANSACTION && pDMNode->element_attrib.global_trans_b == FALSE &&
                  command_list_p->state == COMMAND_EXECUTED  && command_id == CM_CMD_ADD_PARAMS)
            {
               if ((return_value = je_get_key_nvpair_arr (dm_path_p,
                           &command_list_p->command_p->nv_pair_array,
                           &key_nv_pairs)) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Get Key NvPair Array Failed");
                  return OF_FAILURE;
               }
               uiPathLen = of_strlen(dm_path_p);
               parent_path_p = (char *)of_calloc(1,uiPathLen+1);

               cm_remove_last_node_instance_from_dmpath(dm_path_p,parent_path_p);

               if ((return_value = je_add_instance_map_entry (parent_path_p, &key_nv_pairs)) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("JE Add Instance Map Entry  failure");
#if 0 /* to be fixed */ 
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                        return_value, dm_path_p);
                  je_free_nvpair_array (&key_nv_pairs);
                  of_free(parent_path_p);
                  return OF_FAILURE;
#endif
               }

               command_list_p->del_instance_tree_b = 1;
#ifdef CM_ROLES_PERM_SUPPORT
               if ((return_value = cm_je_set_role_to_instance (parent_path_p,
                           config_session_p->admin_role,
                           &key_nv_pairs)) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("JE Set Role Permissions  failure");
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                        return_value, dm_path_p);
                  je_free_nvpair_array (&key_nv_pairs);
                  of_free(parent_path_p);
                  return OF_FAILURE;
               }
#endif
               je_free_nvpair_array (&key_nv_pairs);
               of_free(parent_path_p);
            }
         }
      }
   }
   CM_JE_DEBUG_PRINT ("Successfuly Executed");
   return OF_SUCCESS;
}

#define MAX_TREE_DEPTH 10

 int32_t je_execute_done_command( struct je_config_session *config_session_p,
      struct je_command_list *pInCommandList,
      uint32_t command_id,
      struct cm_result *result_p)
{
   struct je_command_list *command_list_p1 = NULL;
   struct cm_dm_data_element *pDMNode = NULL;
   UCMDllQNode_t *pDllQNode = NULL;

   char *dm_path_p=NULL;
   char *pCurrentPath=NULL;
   char *parent_path_p=NULL;
   char *dm_path_ps[MAX_TREE_DEPTH]={NULL};
   unsigned char bGlobalParent = FALSE;
   int32_t iParNodes = -1;
   int32_t return_value,ii;
   unsigned char bFound=FALSE;

   dm_path_p = pInCommandList->command_p->dm_path_p;
   pCurrentPath = dm_path_p;

   while (bGlobalParent == FALSE)
   {  
      parent_path_p = (char *)of_calloc(1,of_strlen(pCurrentPath));
      if((return_value = je_get_parent_path(pCurrentPath,parent_path_p)) != OF_SUCCESS)
      {
         of_free(parent_path_p);
         for(ii= iParNodes ; ii >= 0 ; ii--)
         {
            of_free(dm_path_ps[ii]);
         }
         return OF_FAILURE;
      }

      pDMNode = (struct cm_dm_data_element *)
         cmi_dm_get_dm_node_from_instance_path (parent_path_p, of_strlen (parent_path_p));
      if (!pDMNode)
      {
         CM_JE_DEBUG_PRINT ("pDMNode is NULL");
         for(ii= iParNodes ; ii >= 0 ; ii--)
         {
            of_free(dm_path_ps[ii]);
         }
         return OF_FAILURE; 
      }

      if(pDMNode->element_attrib.global_trans_b == 1)
      { 
         bGlobalParent = TRUE;
      }
      else 
      {
         dm_path_ps[iParNodes+1] = parent_path_p;
         iParNodes++;
      }
      pCurrentPath = parent_path_p;
   }

   for(ii = iParNodes;ii >= 0; ii--)
   {
      CM_DLLQ_SCAN (&(config_session_p->pCache->xtn_config_list), pDllQNode,
            UCMDllQNode_t *)
      {
         command_list_p1 =
            CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
         if (!strcmp (command_list_p1->command_p->dm_path_p, dm_path_ps[ii]))
         {
            if(command_list_p1->exec_command_b == TRUE)
               continue;
            bFound=TRUE;
            if(je_execute_command(config_session_p,
                     command_list_p1,
                     command_list_p1->command_p->command_id,
                     result_p) != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT("command  execution failed");
               for(ii= iParNodes ; ii >= 0 ; ii--)
               {
                  of_free(dm_path_ps[ii]);
               }
               return OF_FAILURE;
            }
            command_list_p1->exec_command_b = TRUE;
         }
      }

      if(bFound == FALSE)
      {
         for(ii= iParNodes ; ii >= 0 ; ii--)
         {
            of_free(dm_path_ps[ii]);
         }
      }
      bFound = FALSE;
   }

   if(je_execute_command(config_session_p,
            pInCommandList,
            pInCommandList->command_p->command_id,
            result_p) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT("command  execution failed");
      return OF_FAILURE;
   }
   pInCommandList->exec_command_b = TRUE;
   return OF_SUCCESS;
}


/**
  \ingroup JEAPI
  This API calls delete functions to delete  Security application data for each
  node for entire subtree from DM Path for entire sub tree deletes dm instance
  nodes also. 
  <b>Input paramameters: </b>\n
  <b><i>dm_path_p:</i></b> Pointer to Data Model Path
  <b><i>pNvPairAry:</i></b> Pointer to Nv Pair Array
  <b>Output Parameters: </b>
  <b><i>result_p</i></b> double pointer to UCMResult
  In Failure case error information and in success case none.  
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cmje_delete_subtree 
 ** Description   : This API calls delete functions to delete  Security 
 **                  application data for each node for entire subtree from DM Path
 **                  for entire sub tree deletes dm instance nodes also.
 ** Input params  : dm_path_p  - Pointer to Data Model Path
 **               : pNvPairAry  - Pointer to Nv Pair Array
 ** Output params : result_p - double pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
#ifdef CM_DM_INSTANCE_TREE_DEPENDENCY_ENABLED
int32_t cmje_delete_subtree (char * dm_path_p,
      struct cm_array_of_nv_pair * pnv_pair_array,
      struct cm_result * result_p,state)
{
   struct cm_dm_data_element *pDMNode = NULL;
   UCMDM_Inode_t *pDMInode = NULL;
   UCMDM_Instance_t *pInstanceNode = NULL;
   struct cm_iv_pair KeyIvPair = { };
   struct cm_array_of_iv_pairs mand_iv_pairs = { };
   struct cm_array_of_iv_pairs opt_iv_pairs = { };
   struct cm_array_of_nv_pair *dmpath_key_nv_pairs_p = NULL;
   int32_t return_value;
   int32_t iInodeRet;
   void *opaque_info_p = NULL;
   UCMDllQNode_t *pDllQNode, *pTmp;
   UCMDllQ_t *pDll;
   char *pINodeDMPath = NULL;
   struct cm_nv_pair *dm_instance_key_nv_pair_p = NULL;

   CM_JE_DEBUG_PRINT ("Entered");
   if (!dm_path_p || of_strlen(dm_path_p) == 0)
   {
      CM_JE_DEBUG_PRINT ("DM Path is NULL");
      result_p->result.error.error_code = UCMJE_ERROR_DMPATH_NULL;
      return OF_FAILURE;
   }

   pDMNode =
      (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path (dm_path_p,
            of_strlen
            (dm_path_p));
   if (!pDMNode)
   {
      CM_JE_DEBUG_PRINT ("pDMNode is NULL");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_GETNODE_BYDMPATH,
            dm_path_p);
      return OF_FAILURE;
   }

   /* Get Kes from DM Path */
   if ((return_value = cmi_dm_get_key_array_from_namepath (dm_path_p,
               &dmpath_key_nv_pairs_p))
         != OF_SUCCESS)
   {
      if(return_value == OF_FAILURE)
      {
         return_value = UCMJE_ERROR_DMGET_KEYS_FROM_NAMEPATH_FAILED;
      }

      CM_JE_DEBUG_PRINT ("Keys Iv Array From Name Path failed");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            return_value,
            dm_path_p);
      return OF_FAILURE;
   }

   do
   {
      if (iInodeRet = cmi_dm_get_first_inode_using_instance_path (dm_path_p,
               UCMDM_TRAVERSAL_TYPE_BOTTOM_TO_TOP,
               &pDMInode,
               &opaque_info_p) !=
            OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("GetFirstInode FAILED.. ignoring Failure");
         break;
      }

      if (unlikely (pDMInode->pDMNode == NULL))
      {
         CM_JE_DEBUG_PRINT ("Inode's DM Node is NULL");
         return_value = OF_FAILURE;
         break;
      }

      CM_JE_DEBUG_PRINT ("Inode = %s", pDMInode->pDMNode->name_p);

      pINodeDMPath = cmi_dm_create_name_path_from_node (pDMInode->pDMNode);
      if (!pINodeDMPath)
      {
         CM_JE_DEBUG_PRINT ("Create Name Path From Node failed");
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
               UCMJE_ERROR_CREATE_NAMEPATH_FROM_DMNODE_FAILED,
               dm_path_p);

         return_value = OF_FAILURE;
         break;
      }

      pDll = &pDMInode->ChildAccess.InstanceList;

      /* This is the Last Inode to be deleted */
      if ((pDMInode->pDMNode == pDMNode) &&
            (pDMInode->pDMNode->element_attrib.element_type ==
             CM_DMNODE_TYPE_TABLE))
      {
         CM_JE_DEBUG_PRINT ("This is same Inode requested for DM Path");

         /*If pnv_pair_array is NULL  it is a factory Reset 
          * else delete command
          */
         if (pnv_pair_array)
         {
            /* Nv Pairs passed.. it is not a factory reset command */
            /* Its a  Delete Command */
            if ((return_value = je_make_ivpairs (dm_path_p, NULL,
                        dmpath_key_nv_pairs_p, pnv_pair_array,
                        &mand_iv_pairs, &opt_iv_pairs,
                        &KeyIvPair, result_p)) != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Convertion to IV Pairs Failed");
               return_value = OF_FAILURE;
               break;
            }
            /*
               if ((return_value =
               cmje_execute_delete_command (pDMNode, &mand_iv_pairs,
               result_p,0)) != OF_SUCCESS)
               {
               CM_JE_DEBUG_PRINT ("Execute Delete Command Failed for Key=%s",
               (char *) KeyIvPair.value_p);
               return_value = OF_FAILURE;
               break;
               } */
            if ((return_value = cmi_dm_delete_instance_using_key (pDMInode, KeyIvPair.value_p,
                        KeyIvPair.value_type)) !=
                  OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Delete Instance Using Key failed");
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                     UCMJE_ERROR_DELETE_INSTANCE_FAILED,
                     dm_path_p);
               return_value = OF_FAILURE;
               break;
            }
            CM_JE_DEBUG_PRINT ("Deleted from Instance Tree Node=%s Key=%s",
                  pDMNode->name_p, (char *) KeyIvPair.value_p);
            return_value = OF_SUCCESS;
            break;

         }
         /*No Key NameValue Pairs passed
          * might have issued "factory-reset" command
          */
         else
         {
            /* Its a Factory Reset Command */
            CM_DLLQ_DYN_SCAN (pDll, pDllQNode, pTmp, UCMDllQNode_t *)
            {
               pInstanceNode = CM_DLLQ_LIST_MEMBER
                  (pDllQNode, UCMDM_Instance_t *, list_node);
               if (CM_DLLQ_COUNT (&(pInstanceNode->ChildInodeList)) > 0)
               {
                  /* Do not delete an Instance Node prior to its child
                  */
                  CM_JE_DEBUG_PRINT ("do not delete this node prior to its child");
                  /* Here some thing gone wrong????
                   * If it is a last node how is it possible ChildInodeList > 0
                   */
                  return_value = OF_FAILURE;
                  break;
               }
               dm_instance_key_nv_pair_p =
                  (struct cm_nv_pair *) of_calloc (1, sizeof (struct cm_nv_pair));
               if (!dm_instance_key_nv_pair_p)
               {
                  CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                        UCMJE_ERROR_MEMORY_ALLOCATION_FAILED,
                        dm_path_p);
                  return_value = OF_FAILURE;
                  break;
               }
               if ((return_value =
                        cmi_dm_create_nv_pair_from_imap_entry_and_dm_node (&pInstanceNode->Map,
                           pDMInode->pDMNode,
                           dm_instance_key_nv_pair_p))
                     != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Create Nv Pair from Instance Entry Failed");
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                        UCMJE_ERROR_DELETE_INODE_FAILED,
                        dm_path_p);
                  return_value = OF_FAILURE;
                  break;
               }
               if (dmpath_key_nv_pairs_p->count_ui > 0)
               {
                  /* find in DM Path last token is Instance??? */
                  if (!of_strcmp (dmpath_key_nv_pairs_p->nv_pairs
                           [dmpath_key_nv_pairs_p->count_ui - 1].name_p,
                           dm_instance_key_nv_pair_p->name_p))
                  {
                     /* Delete only matched instance */
                     if (of_strcmp (dmpath_key_nv_pairs_p->nv_pairs
                              [dmpath_key_nv_pairs_p->count_ui - 1].value_p,
                              dm_instance_key_nv_pair_p->value_p))
                     {
                        cm_je_free_ptr_nvpair (dm_instance_key_nv_pair_p);
                        continue;
                     }
                  }
               }

               /* Combining All Nv Pairs to frame input arguement to delete function
                * dmpath_key_nv_pairs_p - Keys generated from DM Instance Path
                * dm_instance_key_nv_pair_p  - Keys from current Instance
                * opaque_info_p - Keys between DM Path Node to current Instance Node
                */
               if ((return_value = je_make_ivpairs (dm_path_p, dm_instance_key_nv_pair_p,
                           (struct cm_array_of_nv_pair *) opaque_info_p,
                           dmpath_key_nv_pairs_p, &mand_iv_pairs,
                           &opt_iv_pairs, &KeyIvPair,
                           result_p)) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Nv Pairs to IV Pairs failed");
                  return_value = OF_FAILURE;
                  break;
               }

               if ((return_value =
                        cmje_execute_delete_command (pInstanceNode->pInode->pDMNode,
                           &mand_iv_pairs,
                           result_p,0)) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Execute Delete Command Failed  %s",
                        (char *) dm_instance_key_nv_pair_p->value_p);
                  return_value = OF_FAILURE;
                  break;
               }
               if ((return_value = cmi_dm_delete_instance (pInstanceNode)) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Delete Instance Using Key failed");
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                        UCMJE_ERROR_DELETE_INSTANCE_FAILED,
                        dm_path_p);
                  return_value = OF_FAILURE;
                  break;
               }
               CM_JE_DEBUG_PRINT ("Deleted from Instance Tree Node=%s key= %s",
                     dm_instance_key_nv_pair_p->name_p,
                     (char *) dm_instance_key_nv_pair_p->value_p);
               cm_je_free_ptr_nvpair (dm_instance_key_nv_pair_p);
               cm_je_free_ivpair_array (&mand_iv_pairs);
               cm_je_free_ivpair_array (&opt_iv_pairs);
               cm_je_free_ivpair (&KeyIvPair);
            }
         }

      }
      /* Not a last node to be deleted */
      /* Hence all instances need to be deleted for this Inode */
      else if (pDMInode->pDMNode->element_attrib.element_type ==
            CM_DMNODE_TYPE_TABLE)
      {
         /*Here deleting all instances for t his Inode */
         CM_DLLQ_DYN_SCAN (pDll, pDllQNode, pTmp, UCMDllQNode_t *)
         {
            pDllQNode = CM_DLLQ_FIRST (&(pDMInode->ChildAccess.InstanceList));
            pInstanceNode =
               CM_DLLQ_LIST_MEMBER (pDllQNode, UCMDM_Instance_t *, list_node);
            dm_instance_key_nv_pair_p =
               (struct cm_nv_pair *) of_calloc (1, sizeof (struct cm_nv_pair));
            if (!dm_instance_key_nv_pair_p)
            {
               CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
               return_value = OF_FAILURE;
               break;
            }
            if (cmi_dm_create_nv_pair_from_imap_entry_and_dm_node (&pInstanceNode->Map,
                     pDMInode->pDMNode,
                     dm_instance_key_nv_pair_p)
                  != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Create Nv Pair from Instance Entry Failed");
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                     UCMJE_ERROR_CREATE_NVPAIR_FROM_INSTANCE_ENTRY_FAILED,
                     dm_path_p);
               return_value = OF_FAILURE;
               break;
            }
            /* Combining All Nv Pairs to frame input arguement to delete function
             * dmpath_key_nv_pairs_p - Keys generated from DM Instance Path
             * dm_instance_key_nv_pair_p  - Keys from current Instance
             * opaque_info_p - Keys between DM Path Node to current
             * Instance Node
             */

            if (je_make_ivpairs (pINodeDMPath,
                     dm_instance_key_nv_pair_p,
                     (struct cm_array_of_nv_pair *) opaque_info_p,
                     dmpath_key_nv_pairs_p,
                     &mand_iv_pairs,
                     &opt_iv_pairs, &KeyIvPair,
                     result_p) != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Nv Pairs to IV Pairs failed");
               return_value = OF_FAILURE;
               break;
            }

            if ((cmje_execute_delete_command (pInstanceNode->pInode->pDMNode,
                        &mand_iv_pairs,
                        result_p,0)) != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Execute Delete Command Failed for %s",
                     (char *) dm_instance_key_nv_pair_p->value_p);
               return_value = OF_FAILURE;
               break;
            }


            if (cmi_dm_delete_instance (pInstanceNode) != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Delete Instance Using Key failed");
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                     UCMJE_ERROR_DELETE_INSTANCE_FAILED,
                     dm_path_p);
               return_value = OF_FAILURE;
               break;
            }
            CM_JE_DEBUG_PRINT ("Deleted from Instance Tree Node=%s key= %s",
                  dm_instance_key_nv_pair_p->name_p,
                  (char *) dm_instance_key_nv_pair_p->value_p);
            cm_je_free_ptr_nvpair (dm_instance_key_nv_pair_p);
            cm_je_free_ivpair_array (&mand_iv_pairs);
            cm_je_free_ivpair_array (&opt_iv_pairs);
            cm_je_free_ivpair (&KeyIvPair);
         }
      }

      if (return_value == OF_FAILURE)
      {
         break;
      }
      /* Delete Inode if instances are zero */
      /* Delete Inode if it is not a table Node */
      if ((CM_DLLQ_COUNT (&(pDMInode->ChildAccess.InstanceList)) == 0) ||
            (pDMInode->pDMNode->element_attrib.element_type !=
             CM_DMNODE_TYPE_TABLE))
      {
         CM_JE_DEBUG_PRINT ("Inode Instances are zero.. so deleting Inode");
         if (pDMInode->pDMNode->element_attrib.element_type !=
               CM_DMNODE_TYPE_TABLE)
         {
            CM_JE_DEBUG_PRINT ("%s Inode is not a table hence deleting",
                  pDMInode->pDMNode->name_p);
         }

         if ((return_value = cmi_dm_delete_inode (pDMInode)) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Delete INode Failed");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_DELETE_INODE_FAILED,
                  dm_path_p);
            return_value = OF_FAILURE;
            break;
         }
      }
      /* Cleaning Opaque Info */
      cmi_dm_free_instance_opaque_info (opaque_info_p);
      if (pINodeDMPath)
         of_free (pINodeDMPath);
   }
   while ((iInodeRet == OF_SUCCESS) && (return_value != OF_FAILURE));

   cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);

   if (return_value == OF_FAILURE)
   {
      cm_je_free_ivpair_array (&mand_iv_pairs);
      cm_je_free_ivpair_array (&opt_iv_pairs);
      cm_je_free_ivpair (&KeyIvPair);
      cm_je_free_ptr_nvpair (dm_instance_key_nv_pair_p);
      /* Cleaning Opaque Info */
      cmi_dm_free_instance_opaque_info (opaque_info_p);
      if (pINodeDMPath)
         of_free (pINodeDMPath);
      return OF_FAILURE;
   }
   return OF_SUCCESS;
}
#else

int32_t cmje_delete_subtree (char * pHeadDMPath,
      struct cm_array_of_nv_pair * pnv_pair_array,
      struct cm_result * result_p,
      unsigned char bDelRec,
      int32_t state,
      uint32_t mgmt_engine_id)
{
   struct cm_dm_data_element *pHeadDMNode = NULL;
   struct cm_dm_data_element *data_element_p = NULL;
   struct cm_array_of_iv_pairs *pIvPairArray=NULL;
   struct cm_array_of_nv_pair *dmpath_key_nv_pairs_p = NULL;
   struct cm_array_of_iv_pairs *dm_path_keys_iv_array_p = NULL;
   struct cm_array_of_iv_pairs *array_of_keys_p = NULL;
   int32_t return_value;
   UCMDllQNode_t *pDllQNode;
   unsigned char bDMTplDone = FALSE;
   struct je_stack_node *pStackNode=0;
   int32_t error_code = OF_SUCCESS; // CID 547
   uint32_t channel_id_ui, appl_id_ui;
   cm_channel_getfirst_nrecs_cbk_p pChannelGetFirstNRecFn;
   cm_channel_getnext_nrecs_cbk_p pChannelGetNextNRecFn;
   uint32_t uiRecCount = 1;
   struct cm_array_of_iv_pairs *result_iv_array_p = NULL;
   struct cm_array_of_iv_pairs *key_array_iv_pair_p = NULL;
   void *opaque_info_p = NULL;
   uint32_t opaque_info_length = 0;
   unsigned char bIsChildNode = FALSE;
   unsigned char bGotNewNode=FALSE;
   char *dm_path_p;
   struct cm_array_of_nv_pair *key_array_nv_pair_p=NULL;
   struct cm_array_of_nv_pair *result_nv_pairs_p = NULL;
   struct cm_array_of_iv_pairs IvPairArray = { };
   struct cm_array_of_iv_pairs MandIvPairArr = { }; 
   struct cm_array_of_iv_pairs OptIvPairArr = { };
   struct cm_array_of_iv_pairs KeyIvPair = { };


   CM_JE_DEBUG_PRINT ("Entered");
   CM_DLLQ_INIT_LIST (&je_del_stack_list);

   of_memset(&KeyIvPair,0,sizeof(struct cm_array_of_iv_pairs));
   of_memset(&OptIvPairArr,0,sizeof(struct cm_array_of_iv_pairs));
   of_memset(&MandIvPairArr,0,sizeof(struct cm_array_of_iv_pairs));

   dm_path_p=pHeadDMPath;
   if (!dm_path_p || of_strlen(dm_path_p) == 0)
   {
      CM_JE_DEBUG_PRINT ("DM Path is NULL");
      result_p->result.error.error_code = UCMJE_ERROR_DMPATH_NULL;
      return OF_FAILURE;
   }

   pHeadDMNode = (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path
      (dm_path_p, of_strlen (dm_path_p));

   CM_JE_DEBUG_PRINT ("Node Name = %s", pHeadDMNode->name_p);

   /* Get Keys from DM Path */
   if ((return_value = cmi_dm_get_key_array_from_namepath (dm_path_p, &dmpath_key_nv_pairs_p))
         != OF_SUCCESS)
   {
      if(return_value == OF_FAILURE)
      {
         return_value = UCMJE_ERROR_DMGET_KEYS_FROM_NAMEPATH_FAILED;
      }

      CM_JE_DEBUG_PRINT ("Keys Iv Array From Name Path failed");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            return_value, dm_path_p);
      return OF_FAILURE;
   }

   if(state != EXEC_COMPLETE_CMD || 
         pHeadDMNode->element_attrib.non_config_leaf_node_b != FALSE &&
         pHeadDMNode->element_attrib.non_config_leaf_node_b != TRUE) 
   {
      if(pHeadDMNode->element_attrib.non_config_leaf_node_b != FALSE &&
            pHeadDMNode->element_attrib.non_config_leaf_node_b != TRUE)
      {
         state = EXEC_COMPLETE_CMD;
      }

      if ((return_value = je_combine_nvpairs (NULL,
                  dmpath_key_nv_pairs_p,
                  pnv_pair_array,
                  &result_nv_pairs_p)) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Combining Nv Pair Arrays failed");
         cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
         return OF_FAILURE;
      }

      /* Convert Input Nv Pair Array to IV Pair Array */
      if ((je_copy_nvpairarr_to_ivpairarr (dm_path_p,
                  result_nv_pairs_p,
                  &IvPairArray,
                  mgmt_engine_id)) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Convertion to IV Pairs Failed");
         cm_je_free_ivpair_array (&IvPairArray);
         cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
         cm_je_free_ptr_nvpairArray (result_nv_pairs_p);
         return OF_FAILURE;
      }

      if(cmje_execute_delete_command(pHeadDMNode, &IvPairArray, result_p,state) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT("Command to delete record failed");
         cm_je_free_ivpair_array (&IvPairArray);
         cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
         cm_je_free_ptr_nvpairArray (result_nv_pairs_p);
         return OF_FAILURE;
      }  

      cm_je_free_ivpair_array (&IvPairArray);
      cm_je_free_ptr_nvpairArray (result_nv_pairs_p);
      cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
      return OF_SUCCESS;   
   }

   dm_path_keys_iv_array_p =
      (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));

   if ((return_value =
            je_copy_nvpairarr_to_ivpairarr (dm_path_p, dmpath_key_nv_pairs_p,
               dm_path_keys_iv_array_p,mgmt_engine_id)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Nv Pair array to Iv Pair Array failed");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_DMGET_KEYS_FROM_NAMEPATH_FAILED,
            dm_path_p);
      return OF_FAILURE;
   }

   UCMJE_FREE_PTR_NVPAIR_ARRAY(dmpath_key_nv_pairs_p, dmpath_key_nv_pairs_p->count_ui);


   /*If pnv_pair_array is NULL  it is a factory Reset 
    * else delete command
    */
   // cmi_dm_get_first_element_info
   return_value = cmi_dm_get_first_last_child_element_info (dm_path_p, UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
         &data_element_p, &opaque_info_p,
         &opaque_info_length);
   if (!data_element_p)
   {
      CM_JE_DEBUG_PRINT ("pDMNode is NULL");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_GETNODE_BYDMPATH,
            dm_path_p);
      if(opaque_info_p)
         of_free(opaque_info_p); //CID 342
      return OF_FAILURE;
   }

   CM_JE_DEBUG_PRINT (" Node Name = %s", data_element_p->name_p);

   if(pnv_pair_array)
   {
      pIvPairArray =
         (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
      if ((return_value =  je_copy_nvpairarr_to_ivpairarr (dm_path_p, pnv_pair_array,
                  pIvPairArray, mgmt_engine_id)) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Nv Pair array to Iv Pair Array failed");
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
               UCMJE_ERROR_DMGET_KEYS_FROM_NAMEPATH_FAILED,
               dm_path_p);
         of_free(opaque_info_p); //CID 342
         return OF_FAILURE;
      }
      je_create_and_push_stack_node (data_element_p, opaque_info_p,
            opaque_info_length, dm_path_keys_iv_array_p, pIvPairArray);
      return_value =
         //cmi_dm_get_next_element_info
         cmi_dm_get_next_last_child_element_info (pHeadDMPath, UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
               &data_element_p, &opaque_info_p, &opaque_info_length);

      if ( return_value == OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Next Node Name = %s", data_element_p->name_p);
      }
   }
   else
   {
      CM_JE_DEBUG_PRINT("Nv Pairs not passed might be factory reset command");
   }

   while ((return_value == OF_SUCCESS)  || (bDMTplDone == TRUE))
   {
      bGotNewNode=FALSE;
      bIsChildNode = TRUE;
      uiRecCount=1;
      if (bDMTplDone == TRUE)
      {
         if (CM_DLLQ_COUNT (&je_del_stack_list) == 0)
         {
            CM_JE_DEBUG_PRINT("Template Traversal Completed");
            // if(dm_path_p)
            //  of_free(dm_path_p); //CID 343
            return OF_SUCCESS;
         }
      }
      dm_path_p = cmi_dm_create_name_path_from_node (data_element_p);
      if (CM_DLLQ_COUNT (&je_del_stack_list) != 0)
      {
         pDllQNode = CM_DLLQ_LAST (&je_del_stack_list);
         pStackNode =
            CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_stack_node *, list_node);
         bIsChildNode = cmi_dm_is_child_element (dm_path_p, pStackNode->dm_path_p);
         if(bIsChildNode == TRUE)
         {
            CM_JE_DEBUG_PRINT ("%s and %s are in same path", dm_path_p,
                  pStackNode->dm_path_p);
         }
         else
         {
            CM_JE_DEBUG_PRINT ("%s and %s are not in same path", dm_path_p,
                  pStackNode->dm_path_p);
         }
      }

      CM_JE_DEBUG_PRINT ("Name =%s -- path=%s", data_element_p->name_p, dm_path_p);
      if  (( bIsChildNode == TRUE)  && (bDMTplDone == FALSE))
      {
         if (CM_DLLQ_COUNT (&je_del_stack_list) == 0)
         {
            CM_JE_DEBUG_PRINT ("stack is empty");
         }
         switch (data_element_p->element_attrib.element_type)
         {
            case CM_DMNODE_TYPE_ANCHOR:
            case CM_DMNODE_TYPE_SCALAR_VAR:
            case CM_DMNODE_TYPE_INVALID:
               break;
            case CM_DMNODE_TYPE_TABLE:
               /*Temporary fix*/
               if(data_element_p->element_attrib.non_config_leaf_node_b != FALSE)
                  break;
               if(data_element_p->element_attrib.parent_trans_b == TRUE)
                  break;
               array_of_keys_p =
                  (struct cm_array_of_iv_pairs *) je_collect_keys_from_stack (&je_del_stack_list,
                        dm_path_keys_iv_array_p);
               return_value = cm_get_channel_id_and_appl_id (dm_path_p, &appl_id_ui, &channel_id_ui);
               if (return_value != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("GET Channel Application Failed");
                  error_code = UCMJE_ERROR_GETFIRSTCBK_NULL;
                  break;
               }
               CM_JE_DEBUG_PRINT("PATH=%s AppId=%u channelid=%u", dm_path_p, appl_id_ui, channel_id_ui);
               pChannelGetFirstNRecFn =
                  cm_channels_p[channel_id_ui]->cm_getfirst_nrecs;
               if (!pChannelGetFirstNRecFn)
               {
                  CM_JE_DEBUG_PRINT ("GETFirstNRecords Callback is NULL");
                  error_code = UCMJE_ERROR_GETFIRSTCBK_NULL;
                  break;
               }
               return_value =
                  pChannelGetFirstNRecFn (appl_id_ui, array_of_keys_p, &uiRecCount,
                        &result_iv_array_p);
               if (return_value != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Get FistNRecords Failed");
               }
               else
               {
                  CM_JE_DEBUG_PRINT ("Succesfully recieved Get First N Records");
                  CM_JE_PRINT_IVPAIR_ARRAY(result_iv_array_p);
                  key_array_iv_pair_p =
                     (struct cm_array_of_iv_pairs *)je_find_key_ivpair_array (data_element_p, result_iv_array_p);
                  if(je_create_and_push_stack_node (data_element_p, opaque_info_p,
                           opaque_info_length, array_of_keys_p, key_array_iv_pair_p) != OF_SUCCESS)
                  {
                     error_code=OF_FAILURE;
                     UCMJE_FREE_PTR_IVPAIR_ARRAY(result_iv_array_p, result_iv_array_p->count_ui);
                     break;
                  }
                  UCMJE_FREE_PTR_IVPAIR_ARRAY(result_iv_array_p, result_iv_array_p->count_ui);
               }
               if(array_of_keys_p)
               {
                  UCMJE_FREE_PTR_IVPAIR_ARRAY(array_of_keys_p,array_of_keys_p->count_ui);
               }
         }
         if(error_code == OF_FAILURE)
         {
            break;
         }
      }
      //else if  (( bIsChildNode == FALSE)  && (bDMTplDone == FALSE))
      else
      {
         if (CM_DLLQ_COUNT (&je_del_stack_list) == 0)
         {
            CM_JE_DEBUG_PRINT ("stack is empty");
         }
         CM_JE_DEBUG_PRINT ("Getting Next Records for %s", pStackNode->pDMNode->name_p);
         return_value = cm_get_channel_id_and_appl_id (pStackNode->dm_path_p, &appl_id_ui, &channel_id_ui);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("GET Channel Application Failed");
            error_code = UCMJE_ERROR_GETFIRSTCBK_NULL;
            break;
         }
         pChannelGetNextNRecFn =
            cm_channels_p[channel_id_ui]->cm_getnext_nrecs;
         if (!pChannelGetNextNRecFn)
         {
            CM_JE_DEBUG_PRINT ("GETNextNRecords Callback is NULL");
            error_code = UCMJE_ERROR_GETFIRSTCBK_NULL;
            break;
         }
         array_of_keys_p =
            (struct cm_array_of_iv_pairs *) je_collect_keys_from_stack (&je_del_stack_list,
                  dm_path_keys_iv_array_p);
         error_code =
            pChannelGetNextNRecFn (appl_id_ui,array_of_keys_p, pStackNode->pIvPairArr, &uiRecCount,
                  &result_iv_array_p);
         if ((error_code != OF_SUCCESS) ||((pHeadDMNode == pStackNode->pDMNode)  && (pnv_pair_array)))  
         {
            CM_JE_DEBUG_PRINT ("GET Next N Records Callback failed");
            CM_JE_DEBUG_PRINT ("Deleting Instance = %s",
                  (char *) array_of_keys_p->iv_pairs[array_of_keys_p->count_ui -1].value_p);
            if(bDelRec == DEL_RECORD)
            {
               if ((return_value =
                        cmje_execute_delete_command (pStackNode->pDMNode, array_of_keys_p,
                           result_p,state)) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Execute Delete Command Failed for Key=%s",
                        (char *) array_of_keys_p->iv_pairs[array_of_keys_p->count_ui -1].value_p);
                  error_code = OF_FAILURE;
                  break;
               }
            }
            if(pStackNode->pDMInode != NULL)
            {
               key_array_nv_pair_p =
                  (struct cm_array_of_nv_pair *)of_calloc(1,sizeof(struct cm_array_of_nv_pair));


               if ((return_value = je_split_ivpair_arr(pStackNode->dm_path_p, pStackNode->pIvPairArr,
                           &MandIvPairArr, &OptIvPairArr, &KeyIvPair, result_p,0)) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT("je_split_ivpair_arr Failed");
                  cm_je_free_ivpair_array(&MandIvPairArr);
                  cm_je_free_ivpair_array(&OptIvPairArr);
                  cm_je_free_ivpair_array(&KeyIvPair);
                  return OF_FAILURE;
               }


               je_copy_ivpairarr_to_nvpairarr(pStackNode->dm_path_p, &KeyIvPair, key_array_nv_pair_p);

               cm_je_free_ivpair_array(&MandIvPairArr);
               cm_je_free_ivpair_array(&OptIvPairArr);
               cm_je_free_ivpair_array(&KeyIvPair);

               if ((return_value = cmi_dm_delete_instance_using_keyArray (pStackNode->pDMInode,
                           key_array_nv_pair_p)) !=  OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Delete Instance Using Key failed");
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                        UCMJE_ERROR_DELETE_INSTANCE_FAILED,
                        dm_path_p);
                  error_code=UCMJE_ERROR_DELETE_INSTANCE_FAILED;
                  break;
               }
               if(key_array_nv_pair_p)
                  UCMJE_FREE_PTR_NVPAIR_ARRAY(key_array_nv_pair_p, key_array_nv_pair_p->count_ui);  

               if (CM_DLLQ_COUNT (&(pStackNode->pDMInode->ChildAccess.InstanceList)) == 0)
               {
                  CM_JE_DEBUG_PRINT ("Inode Instances are zero.. so deleting Inode");
                  if ((return_value = cmi_dm_delete_inode (pStackNode->pDMInode)) != OF_SUCCESS)
                  {
                     CM_JE_DEBUG_PRINT ("Delete INode Failed");
                     UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                           UCMJE_ERROR_DELETE_INODE_FAILED,
                           dm_path_p);
                     error_code = UCMJE_ERROR_DELETE_INODE_FAILED;
                     break;
                  }
               }
            }
            CM_DLLQ_DELETE_NODE(&je_del_stack_list,&(pStackNode->list_node));
            je_cleanup_stack_node(pStackNode);
            pStackNode=NULL;
            if(result_iv_array_p)
            {
               UCMJE_FREE_PTR_IVPAIR_ARRAY(result_iv_array_p, result_iv_array_p->count_ui);
            }
            bGotNewNode = TRUE;
         }
         else
         {
            if(opaque_info_p)
               of_free(opaque_info_p);
            opaque_info_p =
               (uint32_t *) of_calloc (1, pStackNode->opaque_info_length*  sizeof (uint32_t));
            of_memcpy (opaque_info_p, pStackNode->opaque_info_p,
                  (sizeof (uint32_t) * pStackNode->opaque_info_length));
            opaque_info_length = pStackNode->opaque_info_length;
            CM_JE_DEBUG_PRINT ("Deleting Instance = %s",
                  (char *) array_of_keys_p->iv_pairs[array_of_keys_p->count_ui -1].value_p);
            if(bDelRec == DEL_RECORD)
            {
               if ((return_value = 
                        cmje_execute_delete_command (pStackNode->pDMNode,
                           array_of_keys_p,result_p,state)) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Execute Delete Command Failed for Key=%s",
                        (char *) array_of_keys_p->iv_pairs[array_of_keys_p->count_ui -1].value_p);
                  error_code = OF_FAILURE;
                  break;
               }
            }

            if(pStackNode->pDMInode != NULL)
            {
               key_array_nv_pair_p =
                  (struct cm_array_of_nv_pair *)of_calloc(1,sizeof(struct cm_array_of_nv_pair));
               je_copy_ivpairarr_to_nvpairarr(pStackNode->dm_path_p, pStackNode->pIvPairArr, key_array_nv_pair_p);

               of_memset(&KeyIvPair,0,sizeof(struct cm_array_of_iv_pairs));
               of_memset(&OptIvPairArr,0,sizeof(struct cm_array_of_iv_pairs));
               of_memset(&MandIvPairArr,0,sizeof(struct cm_array_of_iv_pairs));

               if ((return_value = je_split_ivpair_arr(pStackNode->dm_path_p, pStackNode->pIvPairArr,
                           &MandIvPairArr, &OptIvPairArr, &KeyIvPair, result_p, 0)) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT("je_split_ivpair_arr Failed");
                  cm_je_free_ivpair_array(&MandIvPairArr);
                  cm_je_free_ivpair_array(&OptIvPairArr);
                  cm_je_free_ivpair_array(&KeyIvPair);
                  return OF_FAILURE;
               }

               je_copy_ivpairarr_to_nvpairarr(pStackNode->dm_path_p, &KeyIvPair, key_array_nv_pair_p);

               cm_je_free_ivpair_array(&MandIvPairArr);
               cm_je_free_ivpair_array(&OptIvPairArr);
               cm_je_free_ivpair_array(&KeyIvPair);

               if ((return_value = cmi_dm_delete_instance_using_keyArray (pStackNode->pDMInode,
                           key_array_nv_pair_p)) !=  OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Delete Instance Using Key failed");
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                        UCMJE_ERROR_DELETE_INSTANCE_FAILED,
                        dm_path_p);
                  error_code=UCMJE_ERROR_DELETE_INSTANCE_FAILED;
                  break;
               }
               if(key_array_nv_pair_p)
                  UCMJE_FREE_PTR_NVPAIR_ARRAY(key_array_nv_pair_p, key_array_nv_pair_p->count_ui);
            }
            key_array_iv_pair_p =
               (struct cm_array_of_iv_pairs *)je_find_key_ivpair_array (pStackNode->pDMNode, result_iv_array_p);
            je_update_stack_node (pStackNode,dm_path_keys_iv_array_p, key_array_iv_pair_p);
            UCMJE_FREE_PTR_IVPAIR_ARRAY(result_iv_array_p, result_iv_array_p->count_ui);
            return_value =
               //cmi_dm_get_next_element_info
               cmi_dm_get_next_last_child_element_info (pHeadDMPath, UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
                     &data_element_p, &opaque_info_p, &opaque_info_length);
            bGotNewNode=TRUE;
            bDMTplDone=FALSE;
         }
         if(array_of_keys_p)
         {
            UCMJE_FREE_PTR_IVPAIR_ARRAY(array_of_keys_p,array_of_keys_p->count_ui);
         }
      }
#if 0
      else
      {
         CM_JE_DEBUG_PRINT ("Tree Traversal complete");
         if (CM_DLLQ_COUNT (&je_del_stack_list) == 0)
         {
            CM_JE_DEBUG_PRINT ("stack is empty");
         }
         else

         {
            CM_JE_DEBUG_PRINT ("stack elements still exists");
         }
      }
#endif
      /* If we have taken NewNode from stack or we have already taken
       * processed stack node's next node
       * we can skip call to getnextnodeinfo */
      if( bGotNewNode == FALSE)
      {
         return_value =
            //  cmi_dm_get_next_element_info
            cmi_dm_get_next_last_child_table_n_anchor_element_info
            (pHeadDMPath, UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
             &data_element_p, &opaque_info_p, &opaque_info_length);
         if (return_value == OF_FAILURE)
         {
            CM_JE_DEBUG_PRINT ("Tree Completed");
            bDMTplDone = TRUE;
            if (CM_DLLQ_COUNT (&je_del_stack_list) == 0)
            {
               CM_JE_DEBUG_PRINT ("Stack is empty ");
               je_delete_table_stack (&je_del_stack_list);
               if(dm_path_p)
                  of_free(dm_path_p); //CID 343
               return OF_SUCCESS;
            }
         }
      }
      if(dm_path_p)
         of_free(dm_path_p);
      error_code=OF_SUCCESS;
   }

   if(error_code != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("DELETE sub tree failed");
      if(array_of_keys_p)
      {
         UCMJE_FREE_PTR_IVPAIR_ARRAY(array_of_keys_p,array_of_keys_p->count_ui);
      }
      if(key_array_nv_pair_p)
         UCMJE_FREE_PTR_NVPAIR_ARRAY(key_array_nv_pair_p, key_array_nv_pair_p->count_ui);
      je_delete_table_stack(&je_del_stack_list);
      if(dm_path_p)
         of_free(dm_path_p); //CID 343
      of_free(opaque_info_p); //CID 342
      return OF_FAILURE;
   }
   CM_JE_DEBUG_PRINT ("DELETE sub tree completed");
   je_delete_table_stack(&je_del_stack_list);
   return OF_SUCCESS;
}                             /* ( while return_value == OF_SUCCESS) */

#endif /*CM_DM_INSTANCE_TREE_DEPENDENCY_ENABLED */
/**
  \ingroup JEAPI
  This API deletes  Security application data for given DM Node and Key Iv Pair.
  <b>Input paramameters: </b>\n
  <b><i>dm_path_p:</i></b> Pointer to Data Model Path
  <b><i>key_iv_pair_array_p:</i></b> Pointer to Key Iv Pair Array
  <b>Output Parameters: </b>
  <b><i>result_p</i></b> double pointer to UCMResult
  In Failure case error information and in success case none.  
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cmje_execute_delete_command 
 ** Description   : This API deletes  Security application data for given DM Node
 **                  and Key IV Pairs.
 ** Input params  : dm_path_p  - Pointer to Data Model Path
 **               : key_iv_pair_array_p - Pointer to Key Iv Pair Array
 ** Output params : result_p - double pointer to UCMResult 
 **                            In Success case it contains data
 **                            Failure case error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cmje_execute_delete_command (struct cm_dm_data_element * pDMNode,
      struct cm_array_of_iv_pairs *
      key_iv_pair_array_p, struct cm_result * result_p, int32_t state)
{
   cm_channel_start_xtn_cbk_p pChannelBeginFn;
   cm_channel_delete_cbk_p pChannelDELFn;
   cm_channel_end_xtn_cbk_p pChannelEndFn;
   void *xtn_rec = NULL;
   int32_t return_value, icRet;
   uint32_t appl_id_ui, channel_id_ui;
   struct cm_app_result *app_result_p=NULL;
   char *dm_path_p = NULL;
   struct cm_array_of_nv_pair key_nv_pairs = { };
   unsigned char global_trans_b=0;  
   unsigned char parent_trans_b=0;  
   struct je_xtn_entry *xtn_entry_p = NULL;
   unsigned char tmp_path_c[100]={};
   uint32_t uiPathLen=0;
   unsigned char global_trans_bFound=FALSE;

   if (!pDMNode)
   {
      CM_JE_DEBUG_PRINT ("pDMNode is NULL");
      return UCMJE_ERROR_GETNODE_BYDMPATH;
   }

   dm_path_p = cmi_dm_create_name_path_from_node (pDMNode);
   /* Application Callbacks */
#if 0
   if (!(pDMNode->app_cbks_p))
   {
      CM_JE_DEBUG_PRINT ("Application Callbacks are NULL");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_APPCBK_NULL,
            dm_path_p);
      of_free (dm_path_p);
      return OF_FAILURE;
   }
#endif

   if(pDMNode->element_attrib.global_trans_b == 1)
   {
      global_trans_b = 1;
   }

   if(pDMNode->element_attrib.parent_trans_b == 1)
   {
      parent_trans_b = 1;
   }

   icRet = cm_get_channel_id_and_appl_id (dm_path_p, &appl_id_ui, &channel_id_ui);
   if (icRet == OF_FAILURE)
   {
      of_free (dm_path_p);
      return OF_FAILURE;
   }
   /* Application Begin Callback */
   pChannelBeginFn = cm_channels_p[channel_id_ui]->cm_start_xtn;

   /* Application End Callback */
   pChannelEndFn = cm_channels_p[channel_id_ui]->cm_end_xtn;

   if ((!pChannelEndFn && pChannelBeginFn)
         || (pChannelEndFn && !pChannelBeginFn))
   {
      CM_JE_DEBUG_PRINT ("End Function  is NULL");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_INVALID_PAIR_BEGINENDCBKS,
            dm_path_p);
      of_free (dm_path_p);
      return OF_FAILURE;
   }

   pChannelDELFn = cm_channels_p[channel_id_ui]->cm_delete_rec;
   /* Application ADD Callback */
   if (!pChannelDELFn)
   {
      CM_JE_DEBUG_PRINT ("Application Delete callback is NULL");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_DELCBK_NULL,
            dm_path_p);
      of_free (dm_path_p);
      return OF_FAILURE;
   }

   if (pChannelBeginFn)
   {
      if(global_trans_b == 1)
      {
         CM_DLLQ_SCAN(&(UCMJEGloTransaction_g), xtn_entry_p, struct je_xtn_entry*)
         {
            uiPathLen = of_strlen(xtn_entry_p->pcDMPath);
            if(cmi_dm_remove_keys_from_path(xtn_entry_p->pcDMPath,tmp_path_c,uiPathLen ) 
                  != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT("Removing keys from instance path failed");
               of_free (dm_path_p); //CID 341
               return OF_FAILURE;
            }
            if(of_strcmp(dm_path_p,tmp_path_c)== 0)
            {
               if(xtn_entry_p->xtn_rec)
               {
                  xtn_rec = xtn_entry_p->xtn_rec;
                  global_trans_bFound=TRUE;
                  CM_JE_DEBUG_PRINT("Transaction already started");
                  break;
               }
            }
         }
         if ( global_trans_bFound == FALSE)
         {
            CM_JE_DEBUG_PRINT ("Global Trans is true...calling Begin Function");
            xtn_rec = pChannelBeginFn (appl_id_ui, key_iv_pair_array_p, CM_CMD_DEL_PARAMS,&app_result_p);
            if (!xtn_rec)
            {
               CM_JE_DEBUG_PRINT ("Begin Function returned failure");
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                     UCMJE_ERROR_BEGINCBK_FAILED,
                     dm_path_p);
               of_free (dm_path_p);
               return OF_FAILURE;
            }
         }
      }
      else if(parent_trans_b == 1)
      {
#if 0
         CM_JE_DEBUG_PRINT("Parent Trans is TRUE... ignoring delete command");
         of_free (dm_path_p); //CID 341
         return OF_SUCCESS;
#endif
         if((return_value = je_get_trans_rec(dm_path_p,&xtn_rec)) != OF_SUCCESS
               && state != END_TRANSACTION)
         {
            CM_JE_DEBUG_PRINT("Failed to get transaction Record");
            of_free (dm_path_p); //CID 341
            return OF_FAILURE;
         }
      }
      else
      {
         xtn_rec = pChannelBeginFn (appl_id_ui, key_iv_pair_array_p, CM_CMD_DEL_PARAMS,&app_result_p);
         if (!xtn_rec)
         {
            CM_JE_DEBUG_PRINT ("Begin Function returned failure");
#ifdef CM_AUDIT_SUPPORT
            UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_DEL_FAIL,NULL, NULL,
                  CM_CMD_DEL_PARAMS, dm_path_p, NULL, NULL);
#endif
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_BEGINCBK_FAILED,
                  dm_path_p);
            of_free (dm_path_p);
            return OF_FAILURE;
         }
      }
   }

   if( state == EXEC_COMPLETE_CMD || state == EXEC_CBK_FUNC )
   {
      if ((return_value = pChannelDELFn (appl_id_ui, xtn_rec, key_iv_pair_array_p,
                  &app_result_p)) != OF_SUCCESS)
      {
         if(return_value == NO_REGISTERED_CALLBACK_FUNCTION)
         {
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_DELCBK_NULL,
                  dm_path_p);
            if (pChannelBeginFn)
            {
               return_value = pChannelEndFn (appl_id_ui, xtn_rec, 
                     CM_CMD_CONFIG_SESSION_REVOKE,&app_result_p);
            }
         }
         else
         {
            CM_JE_DEBUG_PRINT ("Delete Function Failed");
#ifdef CM_AUDIT_SUPPORT
            UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_DEL_FAIL,NULL, NULL,
                  CM_CMD_DEL_PARAMS, dm_path_p, NULL, NULL);
#endif
            if (pChannelBeginFn)
            {
               return_value = pChannelEndFn (appl_id_ui, xtn_rec, 
                     CM_CMD_CONFIG_SESSION_REVOKE,&app_result_p);
            }
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_DELCBK_FAILED,
                  dm_path_p);
            if ((return_value =  je_copy_ivpairarr_to_nvpairarr (dm_path_p,
                        key_iv_pair_array_p,
                        &key_nv_pairs)) != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Copy NvPair Arr to IVPairArr Failed");
            }
            else
            {
               UCMJE_COPY_NVPAIR_TO_NVPAIR (key_nv_pairs.nv_pairs[0],
                     result_p->result.error.nv_pair);
               result_p->result.error.nv_pair_b = TRUE;
               je_free_nvpair_array (&key_nv_pairs);
            }
            ucmJECopyApresult_pToUCMResult (dm_path_p, app_result_p, result_p);
         }
         of_free (dm_path_p);
         return OF_FAILURE;
      }
      ucmJECopyApresult_pToUCMResult (dm_path_p, app_result_p, result_p);
   }

   /* Application End Callback */
   if(state == EXEC_COMPLETE_CMD || state == END_TRANSACTION )
   {
      if (pChannelEndFn)
      {
         return_value = pChannelEndFn (appl_id_ui, xtn_rec, 
               CM_CMD_CONFIG_SESSION_COMMIT,&app_result_p);
         if (return_value != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("End Function returned failure");
#ifdef CM_AUDIT_SUPPORT
            UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_DEL_FAIL,NULL, NULL,
                  CM_CMD_DEL_PARAMS, dm_path_p, NULL, NULL);
#endif
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_ENDCBK_FAILED,
                  dm_path_p);
            if ((return_value = je_copy_ivpairarr_to_nvpairarr (dm_path_p,
                        key_iv_pair_array_p,
                        &key_nv_pairs)) != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Copy NvPair Arr to IVPairArr Failed");
            }
            else
            {
               UCMJE_COPY_NVPAIR_TO_NVPAIR (key_nv_pairs.nv_pairs[0],
                     result_p->result.error.nv_pair);
               result_p->result.error.nv_pair_b = TRUE;
               je_free_nvpair_array (&key_nv_pairs);
            }
            of_free (dm_path_p);
            return OF_FAILURE;
         }
      }
   }

   if( global_trans_b == 1 && state == END_TRANSACTION )
   {
      CM_JE_DEBUG_PRINT ("Succesfully Deleted %s Record %s ", pDMNode->name_p,(char *)
            key_iv_pair_array_p->iv_pairs[key_iv_pair_array_p->count_ui - 1].value_p);
#ifdef CM_AUDIT_SUPPORT
      UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_DEL, "UCM","UCM",CM_CMD_DEL_PARAMS,
            dm_path_p,(char *)key_iv_pair_array_p->iv_pairs[key_iv_pair_array_p->count_ui - 1].value_p,
            NULL);
#endif
      if(global_trans_b == 1)
      {
         if(xtn_entry_p->pcDMPath != NULL)
         {
            of_free(xtn_entry_p->pcDMPath);
            xtn_entry_p->pcDMPath = NULL;
         }
         CM_DLLQ_DELETE_NODE(&UCMJEGloTransaction_g,(UCMDllQNode_t *)xtn_entry_p);
         of_free(xtn_entry_p);
      }
   }
   of_free (dm_path_p);
   return OF_SUCCESS;
}

/**
  \ingroup JEAPI
  This API Adds a Instance into Data Model Instance Tree using  Data Model Path, Key
  ID and Value Pair.
  <b>Input paramameters: </b>\n
  <b><i>dm_path_p:</i></b> Pointer to Data Model Path
  <b><i>key_iv_pair_p:</i></b> Pointer to ID and Value Pair
  <b>Output Parameters: </b> None \n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_add_instance_map_entry
 ** Description   : This API Adds a Instance into Data Model Instance Tree using
 Data Model Path and Key ID, Value Pair.
 ** Input params  : dm_path_p  - Pointer to Data Model Path
 : key_iv_pair_p - Pointer to ID and Value Pair
 ** Output params : None
 ** Return value  : OF_SUCCESS in Success
 **                 OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t je_add_instance_map_entry (char * dm_path_p,
      struct cm_array_of_nv_pair * key_nv_pairs_p)
{

   struct cm_dm_instance_map InstanceMap = { };
   int32_t return_value, uiCnt;

   if ((!dm_path_p) || (!key_nv_pairs_p))
   {
      CM_JE_DEBUG_PRINT ("DM Path is NULL");
      return OF_FAILURE;
   }

   /* Get New available Instance ID */
   if (je_get_new_instance_id (&InstanceMap.instance_id) == OF_FAILURE)
   {
      CM_JE_DEBUG_PRINT ("Get New Instance ID failure");
      return UCMJE_ERROR_GET_NEW_INSTANCEID_FAILED;
   }
   if (je_set_bit_for_instance_id (InstanceMap.instance_id, TRUE) == OF_FAILURE)
   {
      CM_JE_DEBUG_PRINT ("Set New Instance ID failure");
      return UCMJE_ERROR_SET_NEW_INSTANCEID_FAILED;
   }

#if 0
   /* Set InstanceMap Entry Values */
   InstanceMap.key_type = key_nv_pair_p->value_type;
   InstanceMap.key_length = key_nv_pair_p->value_length;
   InstanceMap.key_p = (char *) of_calloc (1, InstanceMap.key_length + 1);
   if (!InstanceMap.key_p)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation Failed ");
      return OF_FAILURE;
   }
   of_strncpy ((char *) InstanceMap.key_p,
         (char *) key_nv_pair_p->value_p, InstanceMap.key_length);
#endif

   InstanceMap.key_nv_pairs.count_ui=key_nv_pairs_p->count_ui;
   InstanceMap.key_nv_pairs.nv_pairs=(struct cm_nv_pair *) of_calloc(InstanceMap.key_nv_pairs.count_ui,
         sizeof(struct cm_nv_pair));
   for (uiCnt=0; uiCnt < key_nv_pairs_p->count_ui; uiCnt++)
   {
      je_copy_nvpair_to_nvpair(&key_nv_pairs_p->nv_pairs[uiCnt],
            &InstanceMap.key_nv_pairs.nv_pairs[uiCnt]);
   }

   return_value = cmi_dm_add_instance_map_entry_using_name_path (dm_path_p, &InstanceMap);
   if (return_value != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Adding Instance Map Entry failed");
      of_free (InstanceMap.key_p);
      return UCMJE_ERROR_ADD_INSTANCE_MAP_ENTRY_FAILED;
   }

   //  of_free (InstanceMap.key_p);
   return OF_SUCCESS;
}

#ifdef CM_ROLES_PERM_SUPPORT
 int32_t cm_je_set_role_to_instance (char * dm_path_p,
      char * role_name_p,
      struct cm_array_of_nv_pair * key_nv_pair_p)
{
   struct cm_dm_role_permission *pDMRolePerm;
   ucmdm_RolePermission_t *pdmRolePerm;
   UCMDM_Inode_t *pInode;
   UCMDM_Instance_t *pInstance;
   int32_t return_value;
   char aDMTplPath[1024] = { };

   if (!dm_path_p || of_strlen(dm_path_p) == 0)
   {
      CM_JE_DEBUG_PRINT ("DM Path is NULL");
      return OF_FAILURE;
   }

   if (cmi_dm_remove_keys_from_path (dm_path_p, aDMTplPath,
            of_strlen (dm_path_p)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Remove KeyFrom Path Failed");
      return OF_FAILURE;
   }

   pDMRolePerm =
      (struct cm_dm_role_permission *) of_calloc (1, sizeof (struct cm_dm_role_permission));
   if (!pDMRolePerm)
   {
      return OF_FAILURE;
   }

   pdmRolePerm =
      (ucmdm_RolePermission_t *) of_calloc (1, sizeof (ucmdm_RolePermission_t));
   if (!pdmRolePerm)
   {
      of_free (pDMRolePerm);
      return OF_FAILURE;
   }

   of_strncpy (pDMRolePerm->role, role_name_p, UCMDM_ADMIN_ROLE_MAX_LEN);
   of_strncpy (pdmRolePerm->role, role_name_p, UCMDM_ADMIN_ROLE_MAX_LEN);

   /* Use the DM API to read the permissions for the given node and role */
   return_value = cmi_dm_get_permissions (aDMTplPath, pDMRolePerm);
   if (return_value == OF_FAILURE)
   {
      CM_JE_DEBUG_PRINT ("UCMDM_GetPermissionsByRole Failed");
      /* Set InstanceMap Entry Values */
      pdmRolePerm->permissions = CM_PERMISSION_READ_WRITE_ROLEPERM;
      //              return OF_FAILURE;
   }
   else
   {
      pdmRolePerm->permissions = pDMRolePerm->permissions;
   }

   /* Get Parent Node based on input path */
   pInode = cmi_dm_get_inode_by_name_path (dm_path_p);

   pInstance =
      cmi_dm_get_instance_using_keyArray (pInode, key_nv_pair_p);

   return_value = cmi_dm_attach_instance_role_perm_entry (pInstance, pdmRolePerm);
   if (return_value != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Adding Instance Map Entry failed");
      of_free (pdmRolePerm);
      of_free (pDMRolePerm);
      return UCMJE_DMERROR_SET_DM_INSTANCE_ROLE_ATTRIBS_FAILED;
   }

   of_free (pDMRolePerm);
   return OF_SUCCESS;
}
#endif
/**
  \ingroup JEAPI
  This API Sets a Node Value to its registered default value using Data Model Path.

  <b>Input paramameters: </b>\n
  <b><i>dm_path_p:</i></b> Pointer to Data Model Path
  <b><i>arr_nv_pair_p:</i></b> Pointer to Name and Value Pair Array
  <b>Output Parameters: </b> None \n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_set_nvpair_array_to_default_values 
 ** Description   : This API Sets a Node Value to its registered default value
 using Data Model Path.
 ** Input params  : dm_path_p  - Pointer to Data Model Path
 : arr_nv_pair_p - Pointer to Name and Value Pair Array
 ** Output params : None
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t je_set_nvpair_array_to_default_values (char *
      dm_path_p,
      struct cm_array_of_nv_pair
      *
      arr_nv_pair_p,
      struct cm_result * result_p)
{
   uint32_t uiNvCnt = 0;

   for (uiNvCnt = 0; uiNvCnt < arr_nv_pair_p->count_ui; uiNvCnt++)
   {
      if ((je_set_param_to_default
               (dm_path_p, &arr_nv_pair_p->nv_pairs[uiNvCnt], result_p)) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("je_set_param_to_default Failed");
         return OF_FAILURE;
      }
   }
   return OF_SUCCESS;
}

/**
  \ingroup JEAPI
  This API Sets a Node Value to its registered default value using Data Model Path.

  <b>Input paramameters: </b>\n
  <b><i>dm_path_p:</i></b> Pointer to Data Model Path
  <b><i>pNvPair:</i></b> Pointer to Name and Value Pair 
  <b>Output Parameters: </b> None \n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_set_param_to_default
 ** Description   : This API Sets a Node Value to its registered default value
 using Data Model Path.
 ** Input params  : dm_path_p  - Pointer to Data Model Path
 : pNvPair - Pointer to Name and Value Pair 
 ** Output params : None
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t je_set_param_to_default (char * dm_path_p,
      struct cm_nv_pair * pNvPair,
      struct cm_result * result_p)
{
   struct cm_dm_data_element *pNode = NULL;

   CM_JE_DEBUG_PRINT ("Entered");

   pNode =
      (struct cm_dm_data_element *) cmi_dm_get_child_info_by_name (dm_path_p, pNvPair->name_p);
   if (!pNode)
   {
      CM_JE_DEBUG_PRINT ("cmi_dm_get_child_info_by_name failed");
      return OF_FAILURE;
   }
   if (pNode->element_attrib.mandatory_b == TRUE)
   {
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_MANDATORY_PARAM_CANNOT_BE_SET_TO_DEFAULT,
            dm_path_p);
      UCMJE_MEMALLOC_AND_COPY_RESULTSTRING_INTO_UCMRESULT (result_p, pNode->name_p);
      return OF_FAILURE;
   }

   switch (pNode->data_attrib.data_type)
   {
      case CM_DATA_TYPE_INT:
         pNvPair->value_type = CM_DATA_TYPE_INT;
         pNvPair->value_length = sizeof (int32_t);
         pNvPair->value_p = of_calloc (1, sizeof (int32_t));
         if (!(pNvPair->value_p))
         {
            CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (result_p, UCMJE_ERROR_MEMORY_ALLOCATION_FAILED, dm_path_p);
            return OF_FAILURE;
         }

         CM_JE_DEBUG_PRINT ("Default Value is %d for %s",
               pNode->data_attrib.default_value.idefault_value,
               pNvPair->name_p);
         of_memcpy (pNvPair->value_p, &(pNode->data_attrib.default_value.idefault_value),
               sizeof (int32_t));
         break;

      case CM_DATA_TYPE_UINT:
         pNvPair->value_type = CM_DATA_TYPE_UINT;
         pNvPair->value_length = sizeof (uint32_t);
         pNvPair->value_p = of_calloc (1, sizeof (uint32_t));
         if (!(pNvPair->value_p))
         {
            CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (result_p, UCMJE_ERROR_MEMORY_ALLOCATION_FAILED, dm_path_p);
            return OF_FAILURE;
         }
         CM_JE_DEBUG_PRINT ("Default Value is %u for %s",
               pNode->data_attrib.default_value.uidefault_value,
               pNvPair->name_p);
         of_memcpy (pNvPair->value_p, &(pNode->data_attrib.default_value.uidefault_value),
               sizeof (uint32_t));
         break;

      case CM_DATA_TYPE_INT64:
         pNvPair->value_type = CM_DATA_TYPE_INT64;
         pNvPair->value_length = sizeof (int64_t);
         pNvPair->value_p = of_calloc (1, sizeof (int64_t));
         if (!(pNvPair->value_p))
         {
            CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (result_p, UCMJE_ERROR_MEMORY_ALLOCATION_FAILED, dm_path_p);
            return OF_FAILURE;
         }

         CM_JE_DEBUG_PRINT ("Default Value is %ld for %s",
               pNode->data_attrib.default_value.idefault_value,
               pNvPair->name_p);
         of_memcpy (pNvPair->value_p, &(pNode->data_attrib.default_value.idefault_value),
               sizeof (int64_t));
         break;

      case CM_DATA_TYPE_UINT64:
         pNvPair->value_type = CM_DATA_TYPE_UINT64;
         pNvPair->value_length = sizeof (uint64_t);
         pNvPair->value_p = of_calloc (1, sizeof (uint64_t));
         if (!(pNvPair->value_p))
         {
            CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (result_p, UCMJE_ERROR_MEMORY_ALLOCATION_FAILED, dm_path_p);
            return OF_FAILURE;
         }
         CM_JE_DEBUG_PRINT ("Default Value is %lu for %s",
               pNode->data_attrib.default_value.uidefault_value,
               pNvPair->name_p);
         of_memcpy (pNvPair->value_p, &(pNode->data_attrib.default_value.uidefault_value),
               sizeof (uint64_t));
         break;

      case CM_DATA_TYPE_BOOLEAN:
         pNvPair->value_type = CM_DATA_TYPE_BOOLEAN;
         pNvPair->value_length = sizeof (unsigned char);
         pNvPair->value_p = of_calloc (1, sizeof (unsigned char));
         if (!(pNvPair->value_p))
         {
            CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (result_p, UCMJE_ERROR_MEMORY_ALLOCATION_FAILED, dm_path_p);
            return OF_FAILURE;
         }
         CM_JE_DEBUG_PRINT ("Default Value is %d for %s",
               pNode->data_attrib.default_value.uidefault_value,
               pNvPair->name_p);
         of_memcpy (pNvPair->value_p, &(pNode->data_attrib.default_value.uidefault_value),
               sizeof (unsigned char));
         break;

      case CM_DATA_TYPE_STRING:

         if (pNode->data_attrib.default_value.default_string[0] == '\0')
         {
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (result_p, UCMJE_ERROR_SET_DEFAULT_VALUE_FAILED, dm_path_p);
            return OF_FAILURE;
         }
         pNvPair->value_type = CM_DATA_TYPE_STRING;
         pNvPair->value_length = CM_DM_MAX_DEFAULT_ELEMENT_VALLEN;
         pNvPair->value_p = of_calloc (1, CM_DM_MAX_DEFAULT_ELEMENT_VALLEN + 1);
         if (!(pNvPair->value_p))
         {
            CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (result_p, UCMJE_ERROR_MEMORY_ALLOCATION_FAILED, dm_path_p);
            return OF_FAILURE;
         }
         CM_JE_DEBUG_PRINT ("Default Value is %s for %s",
               pNode->data_attrib.default_value.default_string,
               pNvPair->name_p);
         of_memcpy (pNvPair->value_p, pNode->data_attrib.default_value.default_string,
               CM_DM_MAX_DEFAULT_ELEMENT_VALLEN);
         break;

      case CM_DATA_TYPE_STRING_SPECIAL_CHARS:

         if (pNode->data_attrib.default_value.default_string[0] == '\0')
         {
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (result_p, UCMJE_ERROR_SET_DEFAULT_VALUE_FAILED, dm_path_p);
            return OF_FAILURE;
         }
         pNvPair->value_type = CM_DATA_TYPE_STRING_SPECIAL_CHARS;
         pNvPair->value_length = CM_DM_MAX_DEFAULT_ELEMENT_VALLEN;
         pNvPair->value_p = of_calloc (1, CM_DM_MAX_DEFAULT_ELEMENT_VALLEN + 1);
         if (!(pNvPair->value_p))
         {
            CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (result_p, UCMJE_ERROR_MEMORY_ALLOCATION_FAILED, dm_path_p);
            return OF_FAILURE;
         }
         CM_JE_DEBUG_PRINT ("Default Value is %s for %s",
               pNode->data_attrib.default_value.default_string,
               pNvPair->name_p);
         of_memcpy (pNvPair->value_p, pNode->data_attrib.default_value.default_string,
               CM_DM_MAX_DEFAULT_ELEMENT_VALLEN);
         break;

      case CM_DATA_TYPE_DATETIME:
      case CM_DATA_TYPE_BASE64:
      case CM_DATA_TYPE_UNKNOWN:
      default:
         CM_JE_DEBUG_PRINT ("Un handled data type");
         break;

   }
   return OF_SUCCESS;
}

/**
  \ingroup JEAPI
  This API  checks given Node Name in Command Lists of existing Configuratin
  Session. If Already present it overwrites with new value.
  It writes overwritten Pair Id into ReplaceId Array. It increments uiReplacedNvPairCnt
  for each replaced node value.

  <b>Input paramameters: </b>\n
  <b><i>command_list_p:</i></b> Pointer to CommandList in Configuratin Session
  <b><i>pnv_pair_array:</i></b> Pointer to Name and Value Pair Array
  <b>Output Parameters: </b>  \n
  <b><i>puiReplacedId:</i></b> Pointer to Array of Replaced Node Ids
  <b><i>puiReplacedNvPairCnt:</i></b> Pointer to total Number of Replaced Nv pairs.
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_cfgsess_check_dup_values 
 ** Description   : This API Checks given  Name Value Pairs in Command List.
 **                 If already present it overwrites with new value.
 ** Input params  : command_list_p  - Pointer to CommandList in Configuratin Session
 :  pnv_pair_array - Pointer to Name and Value Pair Array
 ** Output params : puiReplacedId - Pointer to Array of Replaced Ids
 **               :  puiRelaceNvPairCnt - pointer to Replace Name Value Pair Count
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/

 int32_t je_cfgsess_check_dup_values (struct je_command_list *
      command_list_p,
      struct cm_array_of_nv_pair
      * pnv_pair_array,
      uint32_t *
      puiReplacedId,
      uint32_t * puiReplacedNvPairCnt)
{
   uint32_t uiNvCnt;
   uint32_t uiCnt;

   for (uiNvCnt = 0; uiNvCnt < pnv_pair_array->count_ui; uiNvCnt++)
   {
      for (uiCnt = 0;
            uiCnt < command_list_p->command_p->nv_pair_array.count_ui; uiCnt++)
      {
         if (!of_strcmp
               (pnv_pair_array->nv_pairs[uiNvCnt].name_p,
                command_list_p->command_p->nv_pair_array.nv_pairs[uiCnt].name_p))
         {
            CM_JE_DEBUG_PRINT ("NvPair repeated name= %s value= %s",
                  pnv_pair_array->nv_pairs[uiNvCnt].name_p,
                  (char *) pnv_pair_array->nv_pairs[uiNvCnt].value_p);
            /* Freeing Previous Value */
            of_free (command_list_p->command_p->nv_pair_array.nv_pairs[uiCnt].value_p);
            /* Copy New Value */

            /* Copy Value Length */
            command_list_p->command_p->nv_pair_array.nv_pairs[uiCnt].value_length =
               pnv_pair_array->nv_pairs[uiNvCnt].value_length;

            command_list_p->command_p->nv_pair_array.nv_pairs[uiCnt].value_p =
               of_calloc (1,
                     command_list_p->command_p->nv_pair_array.nv_pairs[uiCnt].
                     value_length + 1);
            if (!(command_list_p->command_p->nv_pair_array.nv_pairs[uiCnt].value_p))
            {
               CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Value Field");
               return OF_FAILURE;
            }

            /* Copy Value */
            of_memcpy (command_list_p->command_p->nv_pair_array.nv_pairs[uiCnt].value_p,
                  pnv_pair_array->nv_pairs[uiNvCnt].value_p,
                  command_list_p->command_p->nv_pair_array.nv_pairs[uiCnt].
                  value_length);
            /* Save Replaced NvPair Seq No */
            puiReplacedId[(*puiReplacedNvPairCnt)] = uiNvCnt;
            (*puiReplacedNvPairCnt)++;
         }
      }
   }
   return OF_SUCCESS;
}

/**
  \ingroup JEAPI
  This API Adds Name Value pairs to existing command list in configuration session.

  <b>Input paramameters: </b>\n
  <b><i>command_list_p:</i></b> Pointer to CommandList in Configuratin Session
  <b><i>config_request_p:</i></b> Pointer to UCM configuration request
  <b>Output Parameters: </b>None  \n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */

/******************************************************************************
 ** Function Name : je_cfgsess_add_to_exist_commandlist 
 ** Description   : This API Adds Name Value Pairs to existing Command List in
 **                  configuration session.
 ** Input params  : command_list_p  - Pointer to CommandList in Configuratin Session
 :  config_request_p   - Pointer to Configuration Request
 ** Output params : None
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t
je_cfgsess_add_to_exist_commandlist (struct je_command_list *
      command_list_p,
      struct je_config_request * config_request_p)
{
   uint32_t uiReplacedNvPairCnt = 0;
   uint32_t *puiReplacedIds;
   uint32_t uiCopiedNvCnt;
   uint32_t uiCnt;
   uint32_t uiNvCnt;
   struct cm_command *tmp_command_p;

   if ((!config_request_p) || (!command_list_p))
   {
      CM_JE_DEBUG_PRINT ("Invalid input Arguments");
      return OF_FAILURE;
   }

   /* Allocate Memory for New Command */
   tmp_command_p = of_calloc (1, sizeof (struct cm_command));
   if (!tmp_command_p)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation for Command Failed");
      return OF_FAILURE;
   }

   puiReplacedIds =
      (uint32_t *) of_calloc (config_request_p->data.nv_pair_array.count_ui,
            CM_UINT32_SIZE);
   if (!puiReplacedIds)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation for Replaced Ids");
      of_free (tmp_command_p);
      return OF_FAILURE;
   }

   /* Copy Command ID and DMPath */
   tmp_command_p->command_id = command_list_p->command_p->command_id;
   tmp_command_p->dm_path_p = of_calloc (1, of_strlen (config_request_p->dm_path_p) + 1);
   if (!(tmp_command_p->dm_path_p))
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation Failed for DM Path");
      of_free (tmp_command_p);
      of_free(puiReplacedIds);  /*CID 346*/
      return OF_FAILURE;
   }
   of_strncpy (tmp_command_p->dm_path_p, config_request_p->dm_path_p,
         of_strlen (config_request_p->dm_path_p));

   /* Check for any duplicates
    * If found 
    * Overwrite with New Value
    */
   if ((je_cfgsess_check_dup_values (command_list_p,
               &(config_request_p->data.nv_pair_array),
               puiReplacedIds,
               &uiReplacedNvPairCnt)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Check Dup Values failed");
      of_free (puiReplacedIds);
      of_free (tmp_command_p->dm_path_p);
      of_free (tmp_command_p);
      return OF_FAILURE;
   }

   /* Allocate Memory for New Name Value Pair Array */
   tmp_command_p->nv_pair_array.count_ui =
      command_list_p->command_p->nv_pair_array.count_ui +
      config_request_p->data.nv_pair_array.count_ui - uiReplacedNvPairCnt;
   tmp_command_p->nv_pair_array.nv_pairs =
      of_calloc (tmp_command_p->nv_pair_array.count_ui, sizeof (struct cm_nv_pair));

   if (!(tmp_command_p->nv_pair_array.nv_pairs))
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation for NV Pairs Failed ");
      of_free (puiReplacedIds);
      of_free (tmp_command_p->dm_path_p);
      of_free (tmp_command_p);
      return OF_FAILURE;
   }

   /* Copy Previous Name Value Pairs into cache */
   for (uiNvCnt = 0;
         uiNvCnt < command_list_p->command_p->nv_pair_array.count_ui; uiNvCnt++)
   {
      if (je_copy_nvpair_to_nvpair
            (&command_list_p->command_p->nv_pair_array.nv_pairs[uiNvCnt],
             &tmp_command_p->nv_pair_array.nv_pairs[uiNvCnt]) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Nv Pair To Nv Pair Failed");
         of_free (puiReplacedIds);
         of_free (tmp_command_p->dm_path_p);
         of_free (tmp_command_p);
         return OF_FAILURE;
      }
   }

   /* Copy Name Value Pairs from Request */
   for (uiNvCnt = 0, uiCopiedNvCnt = 0;
         (uiNvCnt < config_request_p->data.nv_pair_array.count_ui) &&
         ((uiCopiedNvCnt + uiReplacedNvPairCnt) <
          config_request_p->data.nv_pair_array.count_ui); uiNvCnt++)
   {
      unsigned char bReplaced = FALSE;

      for (uiCnt = 0; uiCnt < uiReplacedNvPairCnt; uiCnt++)
      {
         if (puiReplacedIds[uiCnt] == uiNvCnt)
         {
            bReplaced = TRUE;
            break;
         }
      }

      /* skip replaced Nv Pair */
      if (bReplaced == FALSE)
      {
         CM_JE_DEBUG_PRINT ("Not replaced pair");
         if (je_copy_nvpair_to_nvpair
               (&config_request_p->data.nv_pair_array.nv_pairs[uiNvCnt],
                &tmp_command_p->nv_pair_array.nv_pairs[command_list_p->command_p->
                nv_pair_array.count_ui +
                uiCopiedNvCnt]) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Nv Pair To Nv Pair Failed");
         }
         uiCopiedNvCnt++;
      }
   }

   of_free (puiReplacedIds);
   /* Freeing existing Command */
   je_free_command (command_list_p->command_p);
   /* Assign New Command to Command List */
   command_list_p->command_p = tmp_command_p;

   return OF_SUCCESS;
}

/**
  \ingroup JEAPI
  This API Adds Name Value pairs to existing command list in configuration session.

  <b>Input paramameters: </b>\n
  <b><i>command_list_p:</i></b> Pointer to CommandList in Configuratin Session
  <b><i>config_request_p:</i></b> Pointer to UCM configuration request
  <b>Output Parameters: </b>None  \n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_cfgsess_add_to_exist_commandlist 
 ** Description   : This API Adds Name Value Pairs to New  Command List in
 **                  configuration session.
 ** Input params  : command_list_p  - Pointer to CommandList in Configuratin Session
 **                 config_request_p   - Pointer to Configuration Request
 ** Output params : None
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t je_cfgsess_add_to_new_commandlist (struct je_command_list
      *
      command_list_p,
      struct je_config_request
      * config_request_p)
{
   uint32_t uiNvCnt;
   struct cm_command *tmp_command_p = NULL;

   /* Allocate Memory for New Command */
   tmp_command_p = of_calloc (1, sizeof (struct cm_command));
   if (!tmp_command_p)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation for Command Failed");
      return OF_FAILURE;
   }

   /* Copy Command ID and DMPath */
   tmp_command_p->command_id = config_request_p->command_id;
   tmp_command_p->dm_path_p = of_calloc (1, of_strlen (config_request_p->dm_path_p) + 1);
   if (!(tmp_command_p->dm_path_p))
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation Failed for DM Path");
      of_free (tmp_command_p);
      return OF_FAILURE;
   }
   of_strncpy (tmp_command_p->dm_path_p, config_request_p->dm_path_p,
         of_strlen (config_request_p->dm_path_p));

   /* Allocate Memory for New Name Value Pair Array */
   tmp_command_p->nv_pair_array.count_ui = config_request_p->data.nv_pair_array.count_ui;
   tmp_command_p->nv_pair_array.nv_pairs =
      of_calloc (tmp_command_p->nv_pair_array.count_ui, sizeof (struct cm_nv_pair));

   if (tmp_command_p->nv_pair_array.nv_pairs == NULL)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation for NV Pairs Failed ");
      of_free (tmp_command_p->dm_path_p);
      of_free (tmp_command_p);
      return OF_FAILURE;
   }

   /* Copy Name Value Pairs from Request */
   for (uiNvCnt = 0; uiNvCnt < config_request_p->data.nv_pair_array.count_ui; uiNvCnt++)
   {
      if (je_copy_nvpair_to_nvpair
            (&config_request_p->data.nv_pair_array.nv_pairs[uiNvCnt],
             &tmp_command_p->nv_pair_array.nv_pairs[uiNvCnt]) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Nv Pair to Nv Pair Failed");
         of_free (tmp_command_p->dm_path_p);
         of_free (tmp_command_p);
         return OF_FAILURE;
      }
   }
   command_list_p->command_p = tmp_command_p;
   return OF_SUCCESS;
}

#ifdef CM_VERSION_SUPPORT
/**
  \ingroup JEAPI
  This API Adds all succesfully executed commands to new configuration version
  in version history_t.

  <b>Input paramameters: </b>\n
  <b><i>config_session_p:</i></b> Pointer to Configuratin Session
  <b>Output Parameters: </b>\n
  <b><i>result_p:</i></b> Pointer to UCM Result contains error information
  in failure case.\n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cm_je_add_command_cache_to_config_version 
 ** Description   : This API Adds all succesfully executed commands to new 
 **                  configuration version in version history_t.
 ** Input params  : command_list_p  - Pointer to Configuratin Session
 ** Output params : result_p - Pointer to UCMResult 
 **                      In Failure case it contains error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t
cm_je_add_command_cache_to_config_version (struct je_config_session *
      config_session_p,
      uint64_t * pNewCfgVer,
      struct cm_result * result_p)
{
   char *dm_path_p = NULL;
   unsigned char bNewCfversion = FALSE;
   uint64_t NewCfgversion = 0;
   int32_t return_value;
   struct je_command_list *command_list_p = NULL;
   UCMDllQNode_t *pDllQNode = NULL;
   struct cm_dm_data_element *pDMNode = NULL;

   /* Read Each Command from Command List
    * Add it to version history_t
    */
   CM_DLLQ_SCAN (&(config_session_p->pCache->add_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      /* Add only Successfully Executed Commands to version history_t */
      if ((command_list_p) && (command_list_p->success_b == TRUE))
      {
         dm_path_p = command_list_p->command_p->dm_path_p;
         pDMNode =
            (struct cm_dm_data_element *)
            cmi_dm_get_dm_node_from_instance_path (dm_path_p, of_strlen (dm_path_p));
         if (!pDMNode)
         {
            CM_JE_DEBUG_PRINT ("pDMNode is NULL");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_GETNODE_BYDMPATH,  dm_path_p);
            return OF_FAILURE;
         }
         if(pDMNode->element_attrib.device_specific_b == TRUE)
            break;
         if (bNewCfversion == FALSE)
         {
            if ((return_value = cm_vh_create_new_config_version (&NewCfgversion)) != OF_SUCCESS)
            {
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
                  (result_p, UCMJE_ERROR_GET_NEWCONFIGVERSION_FAILED, dm_path_p);
               return OF_FAILURE;
            }
            bNewCfversion = TRUE;
            if ((run_time_version_g - save_version_g) == CM_MAX_CONSOLIDATE_VERSION)
            {
               result_p->result.success.reserved= UCMJE_MAX_CONSOLIDATED_VERSION_REACHED;
#ifdef CM_AUDIT_SUPPORT
               UCMJEFrameAndSendLogMsg ( CM_MSG_CONFIG_RUNTIME_VERSIONS_REACHED_MAX,
                     config_session_p->admin_name,config_session_p->admin_role,
                     CM_CMD_ADD_PARAMS,dm_path_p, NULL,NULL);
#endif
               Trace_1(CM_TRACE_ID, TRACE_SEVERE, "version history_t will be lost now , issue save command to avoid this\n");
            }
            else if (((run_time_version_g - save_version_g) >= CM_ALERT_MAX_CONSOLIDATE_VERSION) &&
                  ((run_time_version_g - save_version_g) < CM_MAX_CONSOLIDATE_VERSION))
            {
               result_p->result.success.reserved= UCMJE_WARNING_VERSION_HISTORY_LIMIT;
               Trace_1(CM_TRACE_ID, TRACE_SEVERE, "version history_t will be lost, when there is %d difference between runtime and save configurations. To avoid this please issue Save command\n");
            }
         }
         if ((return_value =
                  cm_vh_add_command_to_config_version (NewCfgversion,
                     command_list_p->command_p)) != OF_SUCCESS)
         {
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (result_p, UCMJE_ERROR_ADDCMD_TO_CONFIGVERSION_FAILED, dm_path_p);
            return OF_FAILURE;
         }
      }
   }

   CM_DLLQ_SCAN (&(config_session_p->pCache->modify_config_list), pDllQNode,UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      if ((command_list_p) && (command_list_p->success_b == TRUE))
      {
         dm_path_p = command_list_p->command_p->dm_path_p;
         pDMNode =
            (struct cm_dm_data_element *)
            cmi_dm_get_dm_node_from_instance_path (dm_path_p, of_strlen (dm_path_p));
         if (!pDMNode)
         {
            CM_JE_DEBUG_PRINT ("pDMNode is NULL");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_GETNODE_BYDMPATH,  dm_path_p);
            return OF_FAILURE;
         }
         if(pDMNode->element_attrib.device_specific_b == TRUE)
            break;
         if (bNewCfversion == FALSE)
         {
            if ((return_value = cm_vh_create_new_config_version (&NewCfgversion)) != OF_SUCCESS)
            {
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
                  (result_p, UCMJE_ERROR_GET_NEWCONFIGVERSION_FAILED, dm_path_p);
               return OF_FAILURE;
            }
            bNewCfversion = TRUE;
            if ((run_time_version_g - save_version_g) == CM_MAX_CONSOLIDATE_VERSION)
            {
               result_p->result.success.reserved= UCMJE_MAX_CONSOLIDATED_VERSION_REACHED;
#ifdef CM_AUDIT_SUPPORT
               UCMJEFrameAndSendLogMsg ( CM_MSG_CONFIG_RUNTIME_VERSIONS_REACHED_MAX,
                     config_session_p->admin_name,config_session_p->admin_role,
                     CM_CMD_SET_PARAMS,dm_path_p, NULL,NULL);
#endif
               Trace_1(CM_TRACE_ID, TRACE_SEVERE, "version history_t will be lost now , issue save command to avoid this\n");
            }
            else if (((run_time_version_g - save_version_g) >= CM_ALERT_MAX_CONSOLIDATE_VERSION) &&
                  ((run_time_version_g - save_version_g) < CM_MAX_CONSOLIDATE_VERSION))
            {
               result_p->result.success.reserved= UCMJE_WARNING_VERSION_HISTORY_LIMIT;
               Trace_1(CM_TRACE_ID, TRACE_SEVERE, "version history_t will be lost, when there is %d difference between runtime and save configurations. To avoid this please issue Save command\n");
            }
         }
         if ((return_value =
                  cm_vh_add_command_to_config_version (NewCfgversion,
                     command_list_p->command_p)) != OF_SUCCESS)
         {
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (result_p, UCMJE_ERROR_ADDCMD_TO_CONFIGVERSION_FAILED, dm_path_p);
            return OF_FAILURE;
         }
      }
   }

   CM_DLLQ_SCAN (&(config_session_p->pCache->delete_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      if ((command_list_p) && (command_list_p->success_b == TRUE))
      {
         dm_path_p = command_list_p->command_p->dm_path_p;
         pDMNode =
            (struct cm_dm_data_element *)
            cmi_dm_get_dm_node_from_instance_path (dm_path_p, of_strlen (dm_path_p));
         if (!pDMNode)
         {
            CM_JE_DEBUG_PRINT ("pDMNode is NULL");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_GETNODE_BYDMPATH,  dm_path_p);
            return OF_FAILURE;
         }
         if(pDMNode->element_attrib.device_specific_b == TRUE)
            break;
         if (bNewCfversion == FALSE)
         {
            if ((return_value = cm_vh_create_new_config_version (&NewCfgversion)) != OF_SUCCESS)
            {
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
                  (result_p, UCMJE_ERROR_GET_NEWCONFIGVERSION_FAILED, dm_path_p);
               return OF_FAILURE;
            }
            bNewCfversion = TRUE;
            if ((run_time_version_g - save_version_g) == CM_MAX_CONSOLIDATE_VERSION)
            {
               result_p->result.success.reserved= UCMJE_MAX_CONSOLIDATED_VERSION_REACHED;
#ifdef CM_AUDIT_SUPPORT
               UCMJEFrameAndSendLogMsg ( CM_MSG_CONFIG_RUNTIME_VERSIONS_REACHED_MAX,
                     config_session_p->admin_name,config_session_p->admin_role,
                     CM_CMD_ADD_PARAMS, dm_path_p, NULL,NULL);
#endif
               Trace_1(CM_TRACE_ID, TRACE_SEVERE, "version history_t will be lost now , issue save command to avoid this\n");
            }
            else if (((run_time_version_g - save_version_g) >= CM_ALERT_MAX_CONSOLIDATE_VERSION) &&
                  ((run_time_version_g - save_version_g) < CM_MAX_CONSOLIDATE_VERSION))
            {
               result_p->result.success.reserved= UCMJE_WARNING_VERSION_HISTORY_LIMIT;
               Trace_1(CM_TRACE_ID, TRACE_SEVERE, "version history_t will be lost, when there is %d difference between runtime and save configurations. To avoid this please issue Save command\n");
            }
         }
         if ((return_value =
                  cm_vh_add_command_to_config_version (NewCfgversion,
                     command_list_p->command_p)) != OF_SUCCESS)
         {
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (result_p, UCMJE_ERROR_ADDCMD_TO_CONFIGVERSION_FAILED, dm_path_p);
            return UCMJE_ERROR_ADDCMD_TO_CONFIGVERSION_FAILED;
         }
      }
   }

   CM_DLLQ_SCAN (&(config_session_p->pCache->xtn_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      if ((command_list_p) && (command_list_p->success_b == TRUE))
      {
         dm_path_p = command_list_p->command_p->dm_path_p;
         pDMNode =
            (struct cm_dm_data_element *)
            cmi_dm_get_dm_node_from_instance_path (dm_path_p, of_strlen (dm_path_p));
         if (!pDMNode)
         {
            CM_JE_DEBUG_PRINT ("pDMNode is NULL");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_GETNODE_BYDMPATH,  dm_path_p);
            return OF_FAILURE;
         }
         if(pDMNode->element_attrib.device_specific_b == TRUE)
            break;
         if (bNewCfversion == FALSE)
         {
            if ((return_value = cm_vh_create_new_config_version (&NewCfgversion)) != OF_SUCCESS)
            {
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
                  (result_p, UCMJE_ERROR_GET_NEWCONFIGVERSION_FAILED, dm_path_p);
               return OF_FAILURE;
            }
            bNewCfversion = TRUE;
         }
         if ((run_time_version_g - save_version_g) == CM_MAX_CONSOLIDATE_VERSION)
         {
            result_p->result.success.reserved= UCMJE_MAX_CONSOLIDATED_VERSION_REACHED;
#ifdef CM_AUDIT_SUPPORT
            UCMJEFrameAndSendLogMsg ( CM_MSG_CONFIG_RUNTIME_VERSIONS_REACHED_MAX,
                  config_session_p->admin_name,config_session_p->admin_role,
                  0, dm_path_p, NULL,NULL);
#endif
            Trace_1(CM_TRACE_ID, TRACE_SEVERE, "version history_t will be lost  now, issue save command to avoid this\n");
         }
         else if (((run_time_version_g - save_version_g) >= CM_ALERT_MAX_CONSOLIDATE_VERSION) &&
               ((run_time_version_g - save_version_g) < CM_MAX_CONSOLIDATE_VERSION))
         {
            result_p->result.success.reserved= UCMJE_WARNING_VERSION_HISTORY_LIMIT;
            Trace_1(CM_TRACE_ID, TRACE_SEVERE, "version history_t will be lost if there is %d versions difference between runtime and save configurations. To avoid this please issue Save command\n");
         }
         if ((return_value =
                  cm_vh_add_command_to_config_version (NewCfgversion,
                     command_list_p->command_p)) != OF_SUCCESS)
         {
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (result_p, UCMJE_ERROR_ADDCMD_TO_CONFIGVERSION_FAILED, dm_path_p);
            return UCMJE_ERROR_ADDCMD_TO_CONFIGVERSION_FAILED;
         }
      }
   }
   *pNewCfgVer = NewCfgversion;
   return OF_SUCCESS;
}

#endif

#ifdef CM_AUDIT_SUPPORT
 int32_t cm_generate_audit_log_message (struct je_config_session *
      config_session_p, struct cm_result * result_p)
{
   char *dm_path_p = NULL;
   char *parent_path_p = NULL;
   int32_t return_value;
   int32_t iMsgId;
   uint32_t uiPathLen;
   struct je_command_list *command_list_p = NULL;
   UCMDllQNode_t *pDllQNode = NULL;
   struct cm_array_of_nv_pair key_nv_pairs={};

   /* Read Each Command from Command List
    * Add it to version history_t
    */
   CM_DLLQ_SCAN (&(config_session_p->pCache->add_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      /* Add only Successfully Executed Commands to version history_t */
      if ((command_list_p) && (command_list_p->success_b == TRUE))
      {
         dm_path_p = command_list_p->command_p->dm_path_p;
         uiPathLen = of_strlen(dm_path_p);
         parent_path_p = (char *)of_calloc(1,uiPathLen+1);
         cm_remove_last_node_instance_from_dmpath(dm_path_p,parent_path_p);

         if ((return_value = je_get_key_nvpair_arr (dm_path_p,
                     &command_list_p->command_p->nv_pair_array,
                     &key_nv_pairs)) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Get KeyNvPair Failed");
            of_free(parent_path_p);
            return OF_FAILURE;
         }
         //UCMJEGetFriendlyNames(&command_list_p->command_p->nv_pair_array,dm_path_p,result_p);

         UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_ADD,
               config_session_p->admin_name,
               config_session_p->admin_role,
               CM_CMD_ADD_PARAMS, parent_path_p,
               key_nv_pairs.nv_pairs[0].value_p,
               &command_list_p->command_p->nv_pair_array);
         je_free_nvpair_array (&key_nv_pairs);
         of_free(parent_path_p);
      }
   }

   CM_DLLQ_SCAN (&(config_session_p->pCache->modify_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      /* Add only Successfully Executed Commands to version history_t */
      if ((command_list_p) && (command_list_p->success_b == TRUE))
      {
         dm_path_p = command_list_p->command_p->dm_path_p;
         uiPathLen = of_strlen(dm_path_p);
         parent_path_p = (char *)of_calloc(1,uiPathLen+1);
         cm_remove_last_node_instance_from_dmpath(dm_path_p,parent_path_p);

         if ((return_value = je_get_key_nvpair_arr (dm_path_p,
                     &command_list_p->command_p->nv_pair_array,
                     &key_nv_pairs)) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Get KeyNvPair Failed");
            of_free(parent_path_p);
            return OF_FAILURE;
         }

         //UCMJEGetFriendlyNames(&command_list_p->command_p->nv_pair_array,dm_path_p,result_p);
         UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_SET,
               config_session_p->admin_name,
               config_session_p->admin_role,
               CM_CMD_SET_PARAMS, parent_path_p,
               key_nv_pairs.nv_pairs[0].value_p,
               &command_list_p->command_p->nv_pair_array);
         je_free_nvpair_array (&key_nv_pairs);
         of_free(parent_path_p);
      }
   }

   CM_DLLQ_SCAN (&(config_session_p->pCache->xtn_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      /* Add only Successfully Executed Commands to version history_t */
      if ((command_list_p) && (command_list_p->success_b == TRUE))
      {
         dm_path_p = command_list_p->command_p->dm_path_p;
         uiPathLen = of_strlen(dm_path_p);
         parent_path_p = (char *)of_calloc(1,uiPathLen+1);
         cm_remove_last_node_instance_from_dmpath(dm_path_p,parent_path_p);

         if ((return_value = je_get_key_nvpair_arr (dm_path_p,
                     &command_list_p->command_p->nv_pair_array,
                     &key_nv_pairs)) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Get KeyNvPair Failed");
            of_free(parent_path_p);
            return OF_FAILURE;
         }
         if(command_list_p->command_p->command_id == CM_CMD_DEL_PARAMS)
         {
            of_free(parent_path_p);//CID 345
            continue;
         }
         if(command_list_p->command_p->command_id == CM_CMD_ADD_PARAMS)
         {
            iMsgId = JE_MSG_CONFIG_ADD;
         }
         else
         {
            iMsgId = JE_MSG_CONFIG_SET;
         }

         //UCMJEGetFriendlyNames(&command_list_p->command_p->nv_pair_array,dm_path_p,result_p);
         UCMJEFrameAndSendLogMsg (iMsgId,
               config_session_p->admin_name,
               config_session_p->admin_role,
               command_list_p->command_p->command_id, parent_path_p,
               key_nv_pairs.nv_pairs[0].value_p,
               &command_list_p->command_p->nv_pair_array);
         je_free_nvpair_array (&key_nv_pairs);
         of_free(parent_path_p);
      }
   }

   CM_DLLQ_SCAN (&(config_session_p->pCache->delete_config_list), pDllQNode, UCMDllQNode_t *)
   {
      command_list_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_command_list *, list_node);
      /* Add only Successfully Executed Commands to version history_t */
      if ((command_list_p) && (command_list_p->success_b == TRUE))
      {
         dm_path_p = command_list_p->command_p->dm_path_p;
         uiPathLen = of_strlen(dm_path_p);
         parent_path_p = (char *)of_calloc(1,uiPathLen+1);
         cm_remove_last_node_instance_from_dmpath(dm_path_p,parent_path_p);

         if ((return_value = je_get_key_nvpair_arr (dm_path_p,
                     &command_list_p->command_p->nv_pair_array,
                     &key_nv_pairs)) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Get KeyNvPair Failed");
            of_free(parent_path_p);
            return OF_FAILURE;
         }
         //UCMJEGetFriendlyNames(&command_list_p->command_p->nv_pair_array,dm_path_p,result_p);
         UCMJEFrameAndSendLogMsg (JE_MSG_CONFIG_DEL,
               config_session_p->admin_name,
               config_session_p->admin_role,
               CM_CMD_DEL_PARAMS, parent_path_p,
               key_nv_pairs.nv_pairs[0].value_p,
               &command_list_p->command_p->nv_pair_array);
         je_free_nvpair_array (&key_nv_pairs);
         of_free(parent_path_p);
      }
   }

   return OF_SUCCESS;
}
#endif
/**
  \ingroup JEAPI
  This API Deletes a Name Value Pair from given  Command List in configuration session.
  <b>Input paramameters: </b>\n
  <b><i>command_list_p:</i></b> Pointer to CommandList in Configuratin Session
  <b><i>pDelNvPairName:</i></b> Pointer to NvPair's Name field to be deleted 
  from list\n
  <b>Output Parameters: </b>None  \n
  <b><i>result_p</i></b> Pointer to UCMResult which contains failure information
  in failure case.\n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_delete_nvpair_from_commandlist 
 ** Description   : This API Deletes a Name Value Pair from given  Command List in
 **                  configuration session.
 ** Input params  : command_list_p  - Pointer to CommandList in Configuratin Session
 **                 pDelNvPairName  - Pointer to Nv Pair field name
 ** Output params : result_p - Pointer to UCMResult 
 **                      In Failure case it contains error information.
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
    int32_t
je_delete_nvpair_from_commandlist (struct je_command_list * command_list_p,
      char * pDelNvPairName,
      struct cm_result * result_p)
{
   struct cm_command *tmp_command_p = NULL;
   uint32_t uiNvCnt;

   /* Allocate Memory for New Command */
   tmp_command_p = of_calloc (1, sizeof (struct cm_command));
   if (!tmp_command_p)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation for Command Failed");
      result_p->result.error.error_code = UCMJE_ERROR_MEMORY_ALLOCATION_FAILED;
      return OF_FAILURE;
   }

   /* Copy Command ID and DMPath */
   tmp_command_p->command_id = command_list_p->command_p->command_id;
   tmp_command_p->dm_path_p =
      of_calloc (1, of_strlen (command_list_p->command_p->dm_path_p) + 1);
   if (!(tmp_command_p->dm_path_p))
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation Failed for DM Path");
      result_p->result.error.error_code = UCMJE_ERROR_MEMORY_ALLOCATION_FAILED;
      of_free (tmp_command_p);
      return OF_FAILURE;
   }
   of_strncpy (tmp_command_p->dm_path_p,
         command_list_p->command_p->dm_path_p,
         of_strlen (command_list_p->command_p->dm_path_p));

   /* Allocate Memory for New Name Value Pair Array */
   tmp_command_p->nv_pair_array.count_ui =
      command_list_p->command_p->nv_pair_array.count_ui - 1;

   if (tmp_command_p->nv_pair_array.count_ui > 0)
   {
      tmp_command_p->nv_pair_array.nv_pairs =
         of_calloc (tmp_command_p->nv_pair_array.count_ui, sizeof (struct cm_nv_pair));

      if (!(tmp_command_p->nv_pair_array.nv_pairs))
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation for NV Pairs Failed ");
         result_p->result.error.error_code = UCMJE_ERROR_MEMORY_ALLOCATION_FAILED;
         of_free (tmp_command_p->dm_path_p);
         of_free (tmp_command_p);
         return OF_FAILURE;
      }

      /* Copy Previous Name Value Pairs into cache */
      for (uiNvCnt = 0; (uiNvCnt < command_list_p->command_p->nv_pair_array.count_ui - 1)
            && (of_strcmp(pDelNvPairName,
                  command_list_p->command_p->nv_pair_array.nv_pairs[uiNvCnt].name_p));
            uiNvCnt++)
      {
         if (je_copy_nvpair_to_nvpair
               (&command_list_p->command_p->nv_pair_array.nv_pairs[uiNvCnt],
                &tmp_command_p->nv_pair_array.nv_pairs[uiNvCnt]) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Nv Pair to Nv Pair failed");
            result_p->result.error.error_code = UCMJE_ERROR_MEMORY_ALLOCATION_FAILED;
            of_free (tmp_command_p->dm_path_p);
            of_free (tmp_command_p);
            return OF_FAILURE;
         }
      }
   }
   /* Freeing existing Command */
   je_free_command (command_list_p->command_p);
   /* Assign New Command to Command List */
   command_list_p->command_p = tmp_command_p;

   return OF_SUCCESS;
}

 int32_t je_execute_command_test_func (char * dm_path_p,
      struct cm_array_of_nv_pair * pnv_pair_array,
      uint32_t command_id,
      unsigned char referrence_in_cache_b,
      struct cm_result * result_p,
      uint32_t mgmt_engine_id)
{
   struct cm_dm_data_element *pDMNode = NULL;
   int32_t return_value, icRet;
   uint32_t appl_id_ui, channel_id_ui;
   cm_channel_test_cbk_p pChannelTestFn;
   struct cm_array_of_iv_pairs IvPairArray = { };
   struct cm_app_result *app_result_p = NULL;
   struct cm_array_of_nv_pair *dmpath_key_nv_pairs_p = NULL;
   struct cm_array_of_nv_pair *result_nv_pairs_p = NULL;

   if (!dm_path_p || of_strlen(dm_path_p) == 0)
   {
      CM_JE_DEBUG_PRINT ("DM Path is NULL");
      result_p->result.error.error_code = UCMJE_ERROR_DMPATH_NULL;
      return OF_FAILURE;
   }

   pDMNode =
      (struct cm_dm_data_element *)
      cmi_dm_get_dm_node_from_instance_path (dm_path_p, of_strlen (dm_path_p));
   if (!pDMNode)
   {
      CM_JE_DEBUG_PRINT ("pDMNode is NULL");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_GETNODE_BYDMPATH,
            dm_path_p);
      return OF_FAILURE;
   }

#if 0
   /* Application Callbacks */
   if (!(pDMNode->app_cbks_p))
   {
      CM_JE_DEBUG_PRINT ("Application Callbacks are NULL");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_APPCBK_NULL,
            dm_path_p);
      return OF_FAILURE;
   }
#endif
   /* Application Callbacks */
   icRet = cm_get_channel_id_and_appl_id (dm_path_p, &appl_id_ui, &channel_id_ui);
   if (icRet == OF_FAILURE)
      return OF_FAILURE;
   //pTestFn = pDMNode->app_cbks_p->cm_test_config;
   pChannelTestFn = cm_channels_p[channel_id_ui]->cm_test_config;
   if (!pChannelTestFn)
   {
      CM_JE_DEBUG_PRINT ("Test Function is NULL");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_TESTCBK_FAILED,
            dm_path_p);
      return OF_FAILURE;
   }

   if ((return_value = cmi_dm_get_key_array_from_namepath (dm_path_p,
               &dmpath_key_nv_pairs_p))
         != OF_SUCCESS)
   {
      if(return_value == OF_FAILURE)
      {
         return_value = UCMJE_ERROR_DMGET_KEYS_FROM_NAMEPATH_FAILED;
      }

      CM_JE_DEBUG_PRINT ("Get Keys Array From Name Path failed");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            return_value, dm_path_p);
      return OF_FAILURE;
   }

   if ((return_value = je_combine_nvpairs (NULL,
               dmpath_key_nv_pairs_p,
               pnv_pair_array,
               &result_nv_pairs_p)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Combining Nv Pair Arrays failed");
      cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
      return OF_FAILURE;
   }

   /* Convert Input Nv Pair Array to IV Pair Array */
   if ((je_copy_nvpairarr_to_ivpairarr (dm_path_p,
               result_nv_pairs_p,
               &IvPairArray,
               mgmt_engine_id)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Convertion to IV Pairs Failed");
      cm_je_free_ivpair_array (&IvPairArray);
      cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
      cm_je_free_ptr_nvpairArray (result_nv_pairs_p);
      return OF_FAILURE;
   }

   /* FIXME:
    * for a Test function Mandatory Iv Pairs are sufficient??
    * or All IvPairs need to be passed?
    */
   if (return_value =
         pChannelTestFn (appl_id_ui, &IvPairArray, command_id,
            &app_result_p) != OF_SUCCESS)
   {
      /* Referrence present in Same Configuration Cache
       * So, Test Function is failed
       * Ignore Failure
       */
      if (referrence_in_cache_b != TRUE)
      {
         CM_JE_DEBUG_PRINT ("Application Test Callback returned failure");
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
               UCMJE_ERROR_TESTCBK_FAILED,
               dm_path_p);
         ucmJECopyApresult_pToUCMResult (dm_path_p, app_result_p, result_p);
         cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
         cm_je_free_ptr_nvpairArray (result_nv_pairs_p);
         cm_je_free_ivpair_array (&IvPairArray);
         return OF_FAILURE;
      }
      else
      {
         CM_JE_DEBUG_PRINT("Reference present in same cache, so ignoring TESTfn failure");
      }
   }
   cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
   cm_je_free_ptr_nvpairArray (result_nv_pairs_p);
   cm_je_free_ivpair_array (&IvPairArray);

   return OF_SUCCESS;
}

int32_t je_execute_command_cbk_func (char * dm_path_p,
      struct cm_array_of_nv_pair * pnv_pair_array,
      uint32_t command_id,
      struct cm_result * result_p,
      int32_t state,
      uint32_t mgmt_engine_id)
{
   struct cm_dm_data_element *pDMNode = NULL;
   void *xtn_rec = NULL;
   struct cm_array_of_nv_pair *dmpath_key_nv_pairs_p = NULL;
   struct cm_array_of_iv_pairs mand_iv_pairs = { };
   struct cm_array_of_iv_pairs opt_iv_pairs = { };
   struct cm_array_of_iv_pairs KeyIvPairArr = { };
   struct cm_app_result *app_result_p = NULL;
   int32_t return_value = 0, icRet = 0;
   uint32_t appl_id_ui, channel_id_ui;
   cm_channel_add_cbk_p pChannelADDFn;
   cm_channel_modify_cbk_p pChannelMODFn;
   cm_channel_start_xtn_cbk_p pBeginFn;
   cm_channel_end_xtn_cbk_p pEndFn;
   unsigned char global_trans_b=0;
   unsigned char parent_trans_b=0;
   struct je_xtn_entry *xtn_entry_p=NULL;

   CM_JE_DEBUG_PRINT ("Entered");
   if (!dm_path_p || of_strlen(dm_path_p) == 0)
   {
      CM_JE_DEBUG_PRINT ("DM Path is NULL");
      result_p->result.error.error_code = UCMJE_ERROR_DMPATH_NULL;
      return OF_FAILURE;
   }

   pDMNode =
      (struct cm_dm_data_element *)
      cmi_dm_get_dm_node_from_instance_path (dm_path_p, of_strlen (dm_path_p));
   if (!pDMNode)
   {
      CM_JE_DEBUG_PRINT ("pDMNode is NULL");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_GETNODE_BYDMPATH,
            dm_path_p);
      return OF_FAILURE;
   }

   if(pDMNode->element_attrib.global_trans_b == 1)
   {
      global_trans_b = 1;
   }
   if(pDMNode->element_attrib.parent_trans_b == 1)
   {
      parent_trans_b = 1;
   }

   // Callback Validations
   icRet = cm_get_channel_id_and_appl_id (dm_path_p, &appl_id_ui, &channel_id_ui);
   if (icRet == OF_FAILURE)
   {
      return OF_FAILURE;
   }

   // Application Callbacks 
   if (!(pDMNode->app_cbks_p))
      if (cm_channels_p[channel_id_ui] == NULL)
      {
         CM_JE_DEBUG_PRINT ("Channel Callbacks are NULL");
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
               UCMJE_ERROR_APPCBK_NULL,
               dm_path_p);
         return OF_FAILURE;
      }

   // Application Begin Callback 
   //pBeginFn = pDMNode->app_cbks_p->cm_start_xtn;
   pBeginFn = cm_channels_p[channel_id_ui]->cm_start_xtn;

   //Application End Callback 
   //pEndFn = pDMNode->app_cbks_p->cm_end_xtn;
   pEndFn = cm_channels_p[channel_id_ui]->cm_end_xtn;

   if ((!pEndFn && pBeginFn) || (pEndFn && !pBeginFn))
   {
      CM_JE_DEBUG_PRINT ("Invalid Beging and End Function pair");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_INVALID_PAIR_BEGINENDCBKS,
            dm_path_p);
      return OF_FAILURE;
   }

   if ((return_value = cmi_dm_get_key_array_from_namepath (dm_path_p,
               &dmpath_key_nv_pairs_p))
         != OF_SUCCESS)
   {
      if(return_value == OF_FAILURE)
      {
         return_value = UCMJE_ERROR_DMGET_KEYS_FROM_NAMEPATH_FAILED;
      }
      CM_JE_DEBUG_PRINT ("Get Keys Array From Name Path failed");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            return_value, dm_path_p);
      return OF_FAILURE;
   }
	 CM_JE_PRINT_NVPAIR_ARRAY(dmpath_key_nv_pairs_p);
   
   if ((return_value = je_make_ivpairs (dm_path_p, NULL,
               dmpath_key_nv_pairs_p,
               pnv_pair_array,
               &mand_iv_pairs,
               &opt_iv_pairs,
               &KeyIvPairArr,
               result_p,
               mgmt_engine_id)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Convertion to IV Pairs Failed");
      cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
      return OF_FAILURE;
   }
   if (pBeginFn)
   {
      if(global_trans_b == 1)
      {
         CM_DLLQ_SCAN(&(UCMJEGloTransaction_g), xtn_entry_p, struct je_xtn_entry*)
         {
            if(of_strcmp(dm_path_p,xtn_entry_p->pcDMPath)== 0)
            {
               if(xtn_entry_p->xtn_rec)
               {
                  xtn_rec = xtn_entry_p->xtn_rec;
                  CM_JE_DEBUG_PRINT("Transaction already started");
                  break;
               }
            }
         }
      }
      else if(parent_trans_b == 1)
      {
         if((return_value = je_get_trans_rec(dm_path_p,&xtn_rec)) != OF_SUCCESS
               && state != END_TRANSACTION)
         {
            CM_JE_DEBUG_PRINT("Failed to get transaction Record");
            return OF_FAILURE;
         }
      }
      else
      {
         xtn_rec = pBeginFn (appl_id_ui, &mand_iv_pairs, command_id,&app_result_p);
         if (!xtn_rec)
         {
            CM_JE_DEBUG_PRINT ("Begin Function returned failure");
            ucmJECopyApresult_pToUCMResult (dm_path_p, app_result_p, result_p);
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_BEGINCBK_FAILED,
                  dm_path_p);
            cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
            cm_je_free_ivpair_array (&mand_iv_pairs);
            cm_je_free_ivpair_array (&opt_iv_pairs);
            cm_je_free_ivpair_array (&KeyIvPairArr);
            return OF_FAILURE;
         }
      }
   }

   if( state == EXEC_COMPLETE_CMD || state == EXEC_CBK_FUNC )
   {
      switch (command_id)
      {
         case CM_CMD_ADD_PARAMS:
            // Call Application ADD Callback 
            //pADDFn = pDMNode->app_cbks_p->cm_add_rec;
            pChannelADDFn = cm_channels_p[channel_id_ui]->cm_add_rec;
            if ((return_value =
                     pChannelADDFn (appl_id_ui, xtn_rec, &mand_iv_pairs,
                        &opt_iv_pairs, &app_result_p)) != OF_SUCCESS)
            {
               if(return_value == NO_REGISTERED_CALLBACK_FUNCTION)
               {
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
                     (result_p, UCMJE_ERROR_ADDCBK_NULL,dm_path_p);
               }
               else
               {
                  CM_JE_DEBUG_PRINT ("Application ADD Callbacks returned Failure");
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
                     (result_p, UCMJE_ERROR_ADDCBK_FAILED, dm_path_p);
                  ucmJECopyApresult_pToUCMResult (dm_path_p, app_result_p, result_p);
               }
               if (global_trans_b == FALSE && parent_trans_b == FALSE
                     && pBeginFn && xtn_rec )
               {
                  return_value = pEndFn (appl_id_ui, xtn_rec, CM_CMD_CONFIG_SESSION_REVOKE,&app_result_p);
               }
               cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
               cm_je_free_ivpair_array (&mand_iv_pairs);
               cm_je_free_ivpair_array (&opt_iv_pairs);
               cm_je_free_ivpair_array (&KeyIvPairArr);
               return OF_FAILURE;
            }
            ucmJECopyApresult_pToUCMResult (dm_path_p, app_result_p, result_p);
            break;
         case CM_CMD_SET_PARAMS:
            // Call Application ADD Callback 
            //pMODFn = pDMNode->app_cbks_p->cm_modify_rec;
            pChannelMODFn = cm_channels_p[channel_id_ui]->cm_modify_rec;
            if ((return_value =
                     pChannelMODFn (appl_id_ui, xtn_rec, &mand_iv_pairs,
                        &opt_iv_pairs, &app_result_p)) != OF_SUCCESS)
            {
               if(return_value == NO_REGISTERED_CALLBACK_FUNCTION)
               {
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
                     (result_p, UCMJE_ERROR_MODCBK_NULL, dm_path_p);
               }
               else
               {
                  CM_JE_DEBUG_PRINT ("Application Modify Callback returned Failure");
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
                     (result_p, UCMJE_ERROR_MODCBK_FAILED, dm_path_p);
                  ucmJECopyApresult_pToUCMResult (dm_path_p, app_result_p, result_p);
               }
               if (global_trans_b == FALSE && parent_trans_b == FALSE
                     && pBeginFn && xtn_rec )
               {
                  return_value = pEndFn (appl_id_ui, xtn_rec, CM_CMD_CONFIG_SESSION_REVOKE,&app_result_p);
               }
               cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
               cm_je_free_ivpair_array (&mand_iv_pairs);
               cm_je_free_ivpair_array (&opt_iv_pairs);
               cm_je_free_ivpair_array (&KeyIvPairArr);
               return OF_FAILURE;
            }
            ucmJECopyApresult_pToUCMResult (dm_path_p, app_result_p, result_p);
            break;
         default:
            CM_JE_DEBUG_PRINT ("Un handled command");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_BEGINCBK_FAILED,
                  dm_path_p);
            cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
            cm_je_free_ivpair_array (&mand_iv_pairs);
            cm_je_free_ivpair_array (&opt_iv_pairs);
            cm_je_free_ivpair_array (&KeyIvPairArr);
            return OF_FAILURE;
      }
   }
   if(state == EXEC_COMPLETE_CMD || state == END_TRANSACTION )
   {
      if (pEndFn)
      {
         return_value = pEndFn (appl_id_ui, xtn_rec, CM_CMD_CONFIG_SESSION_COMMIT,&app_result_p);
         if (return_value != OF_SUCCESS)
         {
            ucmJECopyApresult_pToUCMResult (dm_path_p, app_result_p, result_p);
            CM_JE_DEBUG_PRINT ("End Function returned failure");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_ENDCBK_FAILED,
                  dm_path_p);
            cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
            cm_je_free_ivpair_array (&mand_iv_pairs);
            cm_je_free_ivpair_array (&opt_iv_pairs);
            cm_je_free_ivpair_array (&KeyIvPairArr);
            return OF_FAILURE;
         }
      }
   }


   //Delete node from the list, decrement count and free the memory
   if( global_trans_b == 1 && state == END_TRANSACTION )
   {
      if(xtn_entry_p->pcDMPath != NULL)
      {
         of_free(xtn_entry_p->pcDMPath);
         xtn_entry_p->pcDMPath = NULL;
      }
      CM_DLLQ_DELETE_NODE(&UCMJEGloTransaction_g,(UCMDllQNode_t *)xtn_entry_p);
      of_free(xtn_entry_p);
   }
   //Add Instance Map Entry 
   cm_je_free_ptr_nvpairArray (dmpath_key_nv_pairs_p);
   cm_je_free_ivpair_array (&mand_iv_pairs);
   cm_je_free_ivpair_array (&opt_iv_pairs);
   cm_je_free_ivpair_array (&KeyIvPairArr);

   return OF_SUCCESS;
}

 int32_t je_get_trans_rec(char * dm_path_p,void **xtn_rec)
{
   char *pGlobalParent = NULL;
   int32_t return_value;
   int32_t uiPathLen;
   char tmp_path_c[100]={};
   char tmp_path1_c[100]={};
   struct je_xtn_entry *xtn_entry_p=NULL;

   pGlobalParent = of_calloc(1,of_strlen(dm_path_p)+1);
   if((return_value = je_get_global_parent(dm_path_p,pGlobalParent))!= OF_SUCCESS)
   { 
      CM_JE_DEBUG_PRINT("Get global parent failed") ;
      of_free(pGlobalParent); //CID 340
      return OF_FAILURE;
   }

   uiPathLen = of_strlen(pGlobalParent);
   if(cmi_dm_remove_keys_from_path(pGlobalParent,tmp_path1_c,uiPathLen )
         != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT("Removing keys from instance path failed");
      of_free(pGlobalParent); //CID 340
      return OF_FAILURE;
   }

   CM_DLLQ_SCAN(&(UCMJEGloTransaction_g), xtn_entry_p, struct je_xtn_entry*)
   {
      uiPathLen = of_strlen(xtn_entry_p->pcDMPath);
      if(cmi_dm_remove_keys_from_path(xtn_entry_p->pcDMPath,tmp_path_c,uiPathLen )
            != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT("Removing keys from instance path failed");
         of_free(pGlobalParent); //CID 340
         return OF_FAILURE;
      }
      if(of_strcmp(tmp_path1_c,tmp_path_c)== 0)
      {
         if(xtn_entry_p->xtn_rec)
         {
            *xtn_rec = xtn_entry_p->xtn_rec;
            of_free(pGlobalParent);
            return OF_SUCCESS;
         }
      }
   }
   of_free(pGlobalParent);
   return OF_FAILURE;
}

 int32_t je_get_global_parent(char *dm_path_p, char *pGlobalParent)
{
   char *pParDMPath=NULL;
   int32_t return_value;
   struct cm_dm_data_element *pDMNode = NULL;

   pParDMPath = dm_path_p;
   do
   {
      if((return_value = je_get_parent_path(pParDMPath,pGlobalParent)) != OF_SUCCESS)
      {
         return OF_FAILURE;
      }
      pDMNode = (struct cm_dm_data_element *)
         cmi_dm_get_dm_node_from_instance_path (pGlobalParent, of_strlen (pGlobalParent));
      if (!pDMNode)
      {
         CM_JE_DEBUG_PRINT ("pDMNode is NULL");
         return OF_FAILURE;
      }
      if(pDMNode->element_attrib.global_trans_b == 1)
      { 
         return OF_SUCCESS;
      }
      pParDMPath = pGlobalParent;
   }while(1);
}

 int32_t je_get_parent_path(char *dm_path_p, char *parent_path_p)
{
   int32_t iPathLen;

   iPathLen = of_strlen(dm_path_p);
   while( iPathLen > 0 && dm_path_p[iPathLen-1] != '.')
   {
      if(dm_path_p[iPathLen-1] == '}')
      {
         iPathLen--;
         while(dm_path_p[iPathLen-1] != '{')
            iPathLen--;
      }
      iPathLen--;
   }
   if(iPathLen == 0)
      return OF_FAILURE;
   of_strncpy(parent_path_p,dm_path_p,iPathLen-1);
   parent_path_p[iPathLen-1]='\0';
   return OF_SUCCESS;
}

 int32_t cm_remove_last_node_instance_from_dmpath(char *dm_path_p, char *parent_path_p)
{
   int32_t iPathLen;
   int32_t ii;

   iPathLen = of_strlen(dm_path_p);

   if(dm_path_p[iPathLen - 1] != '}')
   {
      of_strcpy(parent_path_p,dm_path_p);
   }
   else
   {
      ii = iPathLen-1;
      while(dm_path_p[ii] != '{' && ii > 0)
      {
         ii--;
      }
      of_strncpy(parent_path_p,dm_path_p,ii);
      parent_path_p[ii+1]='\0';
   }
   return OF_SUCCESS;
}

#if 0
int32_t UCMJEGetFriendlyNames(struct cm_array_of_nv_pair *pInnv_pair_array,char* dm_path_p,struct cm_result * result_p)
{
   struct cm_dm_data_element *pDMNode = NULL;
   struct cm_dm_node_info *tmp_node_info_p = NULL;
   struct cm_array_of_structs *pChildInfoArr = NULL;
   uint32_t iArrayIndex = 0;
   char *pTemvalue_p=NULL;
   uint32_t uiCnt = 0 ,return_value = 0;
   uint32_t index_i = 0;

   pDMNode = (struct cm_dm_data_element *)
      cmi_dm_get_dm_node_from_instance_path (dm_path_p, of_strlen (dm_path_p));
   if (!pDMNode)
   {
      CM_JE_DEBUG_PRINT ("pDMNode is NULL");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_GETNODE_BYDMPATH,
            dm_path_p);
      return OF_FAILURE;
   }

   return_value = cmi_dm_create_child_info_array (pDMNode, &pChildInfoArr);
   if (unlikely (return_value != OF_SUCCESS))
   {
      return OF_FAILURE;
   }

   while(iArrayIndex < pInnv_pair_array->count_ui) //Loop till end of nv_pair_arrayCount
   {
      uiCnt = 0;
      for (index_i = 0; index_i < pChildInfoArr->count_ui; index_i++)
      {
         tmp_node_info_p = (struct cm_dm_node_info *) (pChildInfoArr->struct_arr_p) + index_i;
         if(tmp_node_info_p->name_p != NULL)
         {
            if(of_strcmp(pInnv_pair_array->nv_pairs[iArrayIndex].name_p,tmp_node_info_p->name_p)==0)
            {
               if(tmp_node_info_p->friendly_name_p!=NULL)
               {
                  pInnv_pair_array->nv_pairs[iArrayIndex].friendly_name_length = of_strlen(tmp_node_info_p->friendly_name_p);
                  pInnv_pair_array->nv_pairs[iArrayIndex].friendly_name_p = (char*)of_calloc(1,of_strlen(tmp_node_info_p->friendly_name_p) + 1);
                  of_strcpy(pInnv_pair_array->nv_pairs[iArrayIndex].friendly_name_p,tmp_node_info_p->friendly_name_p);
               }
               if(tmp_node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM)
               {
                  while(uiCnt < tmp_node_info_p->data_attrib.attr.string_enum.count_ui)
                  {
                     if (!strcmp(pInnv_pair_array->nv_pairs[iArrayIndex].value_p,
                              tmp_node_info_p->data_attrib.attr.string_enum.array[uiCnt]))//Logic to get friendly names
                     {
                        if(tmp_node_info_p->data_attrib.attr.string_enum.aFrdArr[uiCnt] != NULL)
                        {
                           if(of_strlen(tmp_node_info_p->data_attrib.attr.string_enum.aFrdArr[uiCnt])!=0)
                           {
                              pTemvalue_p = tmp_node_info_p->data_attrib.attr.string_enum.aFrdArr[uiCnt];
                              pInnv_pair_array->nv_pairs[iArrayIndex].enum_val_len_ui = of_strlen(pTemvalue_p);
                              pInnv_pair_array->nv_pairs[iArrayIndex].enum_value_p = (char*)of_calloc(1,strlen(pTemvalue_p)+1);
                              of_strcpy(pInnv_pair_array->nv_pairs[iArrayIndex].enum_value_p,pTemvalue_p);
                           }
                        }
                        break;
                     }
                     uiCnt++;
                  }
               }
            }
         }
      }
      iArrayIndex++;
   }
}
#endif

int32_t cm_je_read_notifications(uint32_t mgmt_engine_id, uint32_t uiNotification, void *pData, struct cm_result *result_p)
{
   struct cm_result *pLdsvResult = NULL;
   char *dm_path_p;
   struct je_request_node *pJEPendingRequest;
   UCMDllQNode_t *pDllQNode;
   uint64_t *pversion= (uint64_t* ) pData;
#ifdef  CM_EVENT_NOTIFMGR_SUPPORT
   struct cm_evmgr_event Event;
   time_t t;
   int32_t return_value;
#endif

   CM_JE_DEBUG_PRINT ("Entered");
   switch(uiNotification)
   {
      case CM_LOAD_STARTED_EVENT:
         CM_JE_DEBUG_PRINT ("Succesfully Load started at LDSV");
         CM_JE_DEBUG_PRINT ("New version %llx", *pversion);
         bLoadInProgress_g=TRUE;
         break;
      case CM_LOAD_COMPLETED_EVENT:
         CM_JE_DEBUG_PRINT ("Succesfully Load completed at LDSV");
         CM_JE_DEBUG_PRINT ("New version %llx", *pversion);
         save_version_g=*pversion;
         run_time_version_g=*pversion;
         bLoadInProgress_g=FALSE;
         break;
      case CM_LOAD_FAILED_EVENT:
         CM_JE_DEBUG_PRINT ("Load failed at LDSV");
         CM_JE_DEBUG_PRINT ("failed version %llx", *pversion);
         bLoadInProgress_g=FALSE;
         break;
      case CM_SAVE_STARTED_EVENT:
         CM_JE_DEBUG_PRINT ("Succesfully Save Started at LDSV");
         CM_JE_DEBUG_PRINT ("New version %llx", *pversion);
         bSaveInProgress_g=TRUE;
         break;
      case CM_SAVE_COMPLETED_EVENT:
         CM_JE_DEBUG_PRINT ("Succesfully Save Completed at LDSV");
         CM_JE_DEBUG_PRINT ("New Save version %llx", *pversion);
         save_version_g=*pversion;
         run_time_version_g=*pversion;
         bSaveInProgress_g=FALSE;
         CM_DLLQ_SCAN (&cm_je_request_queue_g, pDllQNode, UCMDllQNode_t *)
         {
            pJEPendingRequest =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_request_node *, list_node);
            if((pJEPendingRequest->config_request_p->command_id ==CM_COMMAND_LDSV_REQUEST ) 
                  && 
                  (pJEPendingRequest->config_request_p->sub_command_id == CM_CMD_SAVE_CONFIG))
            {
               dm_path_p=pJEPendingRequest->config_request_p->dm_path_p;
               pLdsvResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
               if (pLdsvResult == NULL)
               {
                  CM_JE_DEBUG_PRINT ("Memory Allocation failed for result structure");
                  return OF_FAILURE;
               }
               pLdsvResult->result_code = CM_JE_SUCCESS;
               pJEPendingRequest->result_p=pLdsvResult;
            }
         }
         cmje_process_pending_request();
         break;
      case CM_SAVE_FAILED_EVENT:
         CM_JE_DEBUG_PRINT ("Save Failed at LDSV");
         CM_JE_DEBUG_PRINT ("failed Save version %llx", *pversion);
         bSaveInProgress_g=FALSE;
         CM_DLLQ_SCAN (&cm_je_request_queue_g, pDllQNode, UCMDllQNode_t *)
         {
            pJEPendingRequest =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_request_node *, list_node);
            if((pJEPendingRequest->config_request_p->command_id ==CM_COMMAND_LDSV_REQUEST ) 
                  && 
                  (pJEPendingRequest->config_request_p->sub_command_id == CM_CMD_SAVE_CONFIG))
            {
               dm_path_p=pJEPendingRequest->config_request_p->dm_path_p;
               pLdsvResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
               if (pLdsvResult == NULL)
               {
                  CM_JE_DEBUG_PRINT ("Memory Allocation failed for result structure");
                  return OF_FAILURE;
               }
               pLdsvResult->result_code = CM_JE_FAILURE;

               if (dm_path_p)
               {
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (pLdsvResult,
                        UCMJE_ERROR_SAVE_CONFIG_FAILED,
                        dm_path_p);
               }
               else
               {
                  pLdsvResult->result.error.error_code = UCMJE_ERROR_SAVE_CONFIG_FAILED;
               }
               pJEPendingRequest->result_p=pLdsvResult;
            }
         }
         cmje_process_pending_request();
         break;
      case CM_CONFIG_VERSION_UPDATE_EVENT:
         CM_JE_DEBUG_PRINT ("Config UpdateEvent receieved");
         break;
   }

#ifdef  CM_EVENT_NOTIFMGR_SUPPORT
   Event.mgmt_engine_id = mgmt_engine_id;
   Event.event_type = uiNotification;
   Event.date_time = time (&t);
   Event.Event.version_change.version = *pversion;
   if ((return_value = cm_evmgr_generate_event (&Event)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Failed to Notify Event.");
      result_p->result.error.error_code =
         UCMJE_ERROR_NOTIFICATION_CONFIGUPDATE_FAILED;
      return OF_FAILURE;
   }
#endif
   return OF_SUCCESS;
}
#if 0
int32_t ucmJEPreapreInstanceTree (char * dm_path_p)
{
   int32_t return_value;
   int32_t iInstRet;
   uint32_t uiRecordCount = 0;
   uint32_t count_i = 0;
   void *tnsprt_channel_p = NULL;
   struct cm_dm_node_info *DMNodeAttrib = NULL;
   void *opaque_info_p = NULL;
   uint32_t opaque_info_length = 0;
   struct cm_array_of_nv_pair *array_of_keys_p = NULL;
   struct cm_array_of_nv_pair *sec_appl_data_p = NULL;
   struct cm_nv_pair *key_nv_pair_p = NULL;
   unsigned char bTableStackDone = FALSE;
   unsigned char bDMTplTraverseDone = FALSE;
   unsigned char bIsChildNode = TRUE;
   char *pDirPath = NULL;
   char *pTempDirPath = NULL;
   char *pDMInstancePath = NULL;
   char aFullPath[LDSV_DMPATH_SIZE];
   char aBuffer[LDSV_DMPATH_SIZE];
   char *file_name_p;

   UCMDllQNode_t *pDllqNode;
   struct je_stack_node *pStackNode;
   UCMDllQ_t TableStackNode;
   UCMDllQ_t *JEDeleteDllQ = &TableStackNode;
   struct cm_role_info role_info = { "Admin", "Admin" };
   struct cm_array_of_nv_pair *dm_path_array_of_keys_p = NULL;
   struct cm_hash_table *pDictonaryTable = NULL;

   CM_LDSV_DEBUG_PRINT ("dm_path_p=%s", dm_path_p);
   pDictonaryTable = cm_hash_create (MAX_HASH_SIZE);
   if (pDictonaryTable == NULL)
   {
      CM_LDSV_DEBUG_PRINT ("Dictonary Table is not created");
      return OF_FAILURE;
   }

   /*initializing double linked list */
   CM_DLLQ_INIT_LIST (&TableStackNode);
   if (dm_path_p != NULL)
   {
      if ((return_value = cmi_dm_get_key_array_from_namepath (dm_path_p,
                  &dmpath_key_nv_pairs_p))
            != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Get Keys Array From Name Path failed");
         UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
               UCMJE_ERROR_DMGET_KEYS_FROM_NAMEPATH_FAILED,
               dm_path_p);
         return OF_FAILURE;
      }
   }
   pDMNode =
      (struct cm_dm_data_element *)
      cmi_dm_get_dm_node_from_instance_path (dm_path_p, of_strlen (dm_path_p));
   if (!pDMNode)
   {
      CM_JE_DEBUG_PRINT ("pDMNode is NULL");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_GETNODE_BYDMPATH,
            dm_path_p);
      cm_je_free_ptr_nvpairArray (pnv_pair_array);
      return OF_FAILURE;
   }
   if (pDMNode->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
   {
      if (!(pDMNode->app_cbks_p))
      {
         CM_JE_DEBUG_PRINT ("Application Callbacks are NULL");
         error_code = UCMJE_ERROR_APPCBK_NULL;
         break;
      }
      /* Copy Input Keys Array */
      if (error_code = je_copy_nvpairarr_to_ivpairarr (dm_path_p,
               dmpath_key_nv_pairs_p,
               &KeysIvArray) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Copy NvPair Arr to IVPairArr Failed");
         break;
      }
      uiRecCount = 1;
      pGetFirstNRecFn = pDMNode->app_cbks_p->cm_getfirst_nrecs;
      if (!pGetFirstNRecFn)
      {
         CM_JE_DEBUG_PRINT ("GETFistNRecords Callback is NULL");
         error_code = UCMJE_ERROR_GETFIRSTCBK_NULL;
         break;
      }
      iAppCbkRet = pGetFirstNRecFn (&KeysIvArray, &uiRecCount, &result_iv_array_p);
      if (iAppCbkRet != OF_SUCCESS)
         error_code = UCMJE_ERROR_GETFIRSTCBK_FAILED;
      break;

      while (iAppCbkRet == OF_SUCCESS)
      {
         if ((return_value = ucmJEGetKeyIvPair (dm_path_p, result_iv_array_p,
                     &KeyIvPair)) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Execute Command Cbk Failed");
            return OF_FAILURE;
         }
         if (error_code = je_copy_ivpair_to_nvpair (dm_path_p,
                  &KeyIvPair,
                  &KeyNvPair) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Copy NvPair Arr to IVPairArr Failed");
            break;
         }
         if (result_iv_array_p)
         {
            for (uiCnt = 0; uiCnt < uiRecCount; uiCnt++)
            {
               cm_je_free_ivpair_array (&result_iv_array_p[uiCnt]);
            }
            of_free (result_iv_array_p);
         }
         if ((return_value = je_add_instance_map_entry (dm_path_p, &KeyNvPair)) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("JE Add Instance Map Entry  failure");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT
               (result_p, return_value, dm_path_p);
            je_free_nvpair(&KeyNvPair);
            return OF_FAILURE;
         }

         pGetNextNRecFn = pDMNode->app_cbks_p->cm_getnext_nrecs;
         if (!pGetNextNRecFn)
         {
            CM_JE_DEBUG_PRINT ("GETNextNRecords Callback is NULL");
            error_code = UCMJE_ERROR_GETNEXTCBK_NULL;
            break;
         }
         iAppCbkRet =
            pGetNextNRecFn (&KeysIvArray, &KeyIvPair, &uiRecCount, &result_iv_array_p);
         cm_je_free_ivpair (&KeyIvPair);
         je_free_nvpair(&KeyNvPair);
      }
   }
   /* Close XML file */
   return OF_SUCCESS;
}
#endif
#endif /*CM_JE_SUPPORT */
#endif /*CM_SUPPORT */
