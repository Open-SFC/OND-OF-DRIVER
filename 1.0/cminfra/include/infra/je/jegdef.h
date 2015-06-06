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
 * File name: jegdef.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/26/2013
 * Description: This file contains defines and structure
 *              definitions provided by UCM Job Execution engine.
 * 
 */

#ifndef UCMJE_GDEF_H
#define UCMJE_GDEF_H

#define CM_JE_SEARCH_CHILD_IN_COMPLETE_DMPATH 
//#define CM_DM_INSTANCE_TREE_DEPENDENCY_ENABLED


#define  CM_JE_FILE_MAX_LEN 256
#define CM_CP_LIB_PATH "/ucm/lib/cp"
#define CM_SECAPP_LIB_PATH "/igateway/ucm/lib/applications"

/*****************************************************************************
 * * * * * * * * * * * U C M   S U B   C O M M A N D S * * * * * * * * * * * *
 *****************************************************************************/
enum
{
  /* Configuration specific commands */
  CM_CMD_ADD_PARAMS = 1,
  CM_CMD_SET_PARAMS,
  CM_CMD_DEL_PARAMS,
  CM_CMD_CANCEL_TRANS_CMD,
  CM_CMD_EXEC_TRANS_CMD,
  CM_CMD_CANCEL_PREV_CMD,
  CM_CMD_COMPARE_PARAM,        /*7 */
  CM_CMD_GET_CONFIG_SESSION,   /* fixme : what is this? */
  CM_CMD_SET_PARAM_TO_DEFAULT,
  CM_CMD_SET_DEFAULTS,

  /* Configuration Session */
  CM_CMD_CONFIG_SESSION_REVOKE,
  CM_CMD_CONFIG_SESSION_COMMIT,

  /* Load & Save specific commands */
  CM_CMD_LOAD_CONFIG /*13 */ ,
  CM_CMD_SAVE_CONFIG,
  CM_CMD_RESET_TO_FACTORY_DEFAULTS,

  /* Application specific commands */
  CM_CMD_GET_FIRST_N_RECS /*16 */ ,
  CM_CMD_GET_NEXT_N_RECS,
  CM_CMD_GET_EXACT_REC,

  CM_CMD_GET_FIRST_N_LOGS /*19 */ ,
  CM_CMD_GET_NEXT_N_LOGS,
  
  /* Data Model specific commands */
  CM_CMD_GET_DM_NODE_INFO, /*21*/
  CM_CMD_GET_DM_NODE_AND_CHILD_INFO,
  CM_CMD_GET_DM_KEY_CHILD_INFO,
  CM_CMD_GET_DM_KEY_CHILDS,
  CM_CMD_GET_DM_FIRST_NODE_INFO,/*25*/
  CM_CMD_GET_DM_NEXT_NODE_INFO,
  CM_CMD_GET_DM_KEYS_ARRAY,
  CM_CMD_GET_DM_CHILD_NODE_INFO,
  CM_CMD_GET_DM_CHILD_NODE_COUNT,
  CM_CMD_IS_DM_CHILD_NODE,/*30*/
  CM_CMD_GET_DM_NEXT_TABLE_ANCHOR_NODE_INFO,

  /* Role Commands */
  CM_CMD_GET_DM_NODE_ROLE_ATTRIBS,/*32*/
  CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE,
  CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE,
  CM_CMD_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE,/*35*/
  CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS,
  CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE,
  CM_CMD_SET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE,
  CM_CMD_DEL_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE,

 /* Instance Commands */  
   CM_CMD_GET_NEW_DM_INSTANCE_ID,/*40*/
  CM_CMD_ADD_DM_INSTANCE_MAP,
  CM_CMD_GET_DM_INSTANCE_MAPLIST_BY_NAMEPATH,
  CM_CMD_DEL_DM_INSTANCE_MAP,
  CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEY,
  CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEYARRAY,/*45*/

  /*Authentication*/
  CM_SUBCOMMAND_AUTHENTICATE_USER,

  /* Ldsv Commads */
  CM_CMD_GET_SAVE_FILE_NAME,
  CM_CMD_GET_SAVE_DIRECTORY_NAME,

  /* TRANSPORT CHANNEL SUB COMMANDS */
  CM_CMD_SET_INACTIVITY_TIMEOUT,

  /* Debug related commands */
  CM_CMD_GET_PENDING_COMMANDS,
  CM_CMD_GET_FIRST_CONFIG_SESSION_INFO,
  CM_CMD_GET_NEXT_CONFIG_SESSION_INFO,
  
	/* version history_t commands*/
  CM_CMD_GET_DELTA_VERSION_HISTORY,
  CM_CMD_GET_VERSIONS,
  
 	/* Events */
  CM_CMD_NOTIFY_JE,

  /*Command state */
  EXEC_COMPLETE_CMD,
  START_TRANSACTION,
  EXEC_CBK_FUNC,
  IS_IT_OK_FUNC,
  END_TRANSACTION,
  COMMAND_EXECUTED,
  CM_CMD_FLUSH_CONFIG,

  /*Stats specific commands */
#ifdef CM_STATS_COLLECTOR_SUPPORT
  CM_CMD_GET_AGGREGATE_STATS,
  CM_CMD_GET_PER_DEVICE_STATS,
  CM_CMD_GET_AVERAGE_STATS,
  CM_CMD_COALESCAR_RESP,
#endif
  CM_CMD_REBOOT_DEVICE,
  CM_MAX_SUBCMDS
};

enum cm_je_result
{
  CM_JE_SUCCESS,
  CM_JE_FAILURE,
  CM_JE_NO_MORE_RECS,
  CM_JE_MORE_RECS
};
/*****************************************************************************
 * * * * * * * * * * S T R U C T U R E   D E F I N I T I O N S * * * * * * * * 
 *****************************************************************************/
struct cm_role_info
{
  char admin_name[CM_MAX_ADMINNAME_LEN];
  char admin_role[CM_MAX_ADMINROLE_LEN];
};

/* version data structure */
struct cm_version
{ 
	 uint64_t version;     /* major:8, minor:8,revision:16  */
	 struct cm_array_of_commands *command_list_p;   /* Command List*/
};

struct cm_result
{
  uint32_t sub_command_id;            /* add/set/del/getfirst/getnext, etc */
  uint32_t result_code;        /* SUCCESS ERROR_CODE  MORE_RECS NO_MORE_RECS FAILURE */
  union
  {
    struct
    {
      uint32_t reserved;
    } success;

    struct
    {
      uint32_t error_code;
      char *result_string_p;
      char *dm_path_p;
      uint32_t nv_pair_b;
      struct cm_nv_pair nv_pair;
    } error;

  } result;
  union
  {
    void *je_data_p;
    struct
    {
      uint32_t count_ui;         /* Record Count */
      struct cm_version *version_list_p; /* array of name-value pairs */
    } version;
    struct
    {
      uint32_t count_ui;         /* Record Count */
      struct cm_array_of_nv_pair *nv_pair_array; /* array of name-value pairs */
    } sec_appl;
    struct
    {
      uint32_t count_ui;
      struct cm_dm_node_info *node_info_p;
      struct cm_array_of_structs *node_info_array_p;
      struct cm_array_of_nv_pair *dm_node_nv_pairs_p;
      struct cm_dm_instance_map *instance_map_list_p;
      void *opaque_info_p;
      uint32_t opaque_info_length;
    } dm_info;

#ifdef CM_STATS_COLLECTOR_SUPPORT
    struct 
    {
      uint32_t nv_pair_flat_buf_len_ui;
      void *nv_pair_flat_buf_p;
    }stats_data;
#endif 

  } data;
};

/**
\ingroup TRCHNLDS
\struct struct cm_request_msg
The UCMMsgRequestHeader_t structure is used to hold the Generic Header information.
All the message buffers that moves across the Transport Channel must contain this 
header at the beginning of the message.

This will specify Total message length, Command ID, Management Engine ID and Admin
roles and permissions information.

\verbatim
typedef struct UCMRequestMsg_s
{
  uint32_t sub_command_id; 
  unsigned char *dm_path_p; 
  uint32_t count_ui; 
  struct cm_array_of_nv_pair nv_pair_array; 
  struct cm_nv_pair prev_nv_pair;
}struct cm_request_msg;

Parameters:

sub_command_id           This specifies the Sub-Command Indetification.
                    Accepted values are: 
                       CM_CMD_ADD_PARAMS
                       CM_CMD_SET_PARAMS
                       CM_CMD_DEL_PARAMS
                       CM_COMMAND_SET_INACTIVITY_TIMEOUT
                       CM_CMD_GET_FIRST_N_RECS
                       CM_CMD_GET_NEXT_N_RECS
                       CM_CMD_GET_EXACT_REC
                       CM_CMD_SAVE_CONFIG
                       CM_CMD_LOAD_CONFIG
                       CM_CMD_SET_DEFAULTS
                      
                       CM_CMD_GET_DM_NODE_INFO
                       CM_CMD_GET_DM_CHILD_NODE_INFO
                       CM_CMD_GET_DM_CHILD_NODE_COUNT
                       CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE
                       CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE
                       CM_CMD_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE
                       CM_CMD_GET_DM_FIRST_NODE_INFO
                       CM_CMD_GET_DM_NEXT_NODE_INFO
                       
                       CM_CMD_GET_NEW_DM_INSTANCE_ID 
                       CM_CMD_GET_ADD_DM_INSTANCE_MAP
                       CM_CMD_GET_GET_DM_INSTANCE_MAP
                       CM_CMD_GET_DEL_DM_INSTANCE_MAP
                       CM_CMD_GET_GET_DM_INSTANCE_ROLE_ATTRIBS
                       CM_CMD_GET_SET_DM_INSTANCE_ROLE_ATTRIBS
                       CM_CMD_GET_DEL_DM_INSTANCE_ROLE_ATTRIBS
                      
                       CM_CMD_GET_PENDING_COMMANDS
                       CM_CMD_GET_FIRST_CONFIG_SESSION_INFO
                       CM_CMD_GET_NEXT_CONFIG_SESSION_INFO

ulDMPathLength     Specifies the data_model path length.

DMPath             Holds the data_model path.

nv_pairs        Name-Value pair array. Used to hold set of Name-Value pairs.

\endverbatim
*/

struct cm_request_msg
{
  uint32_t sub_command_id;            /* add/set/del/getfirst/getnext, etc */
  unsigned char *dm_path_p;            /*Data Model Path string */
  uint32_t count_ui;             /*Number of Records to Fetch */
  struct cm_array_of_nv_pair nv_pair_array;      /* array of name-value pairs */
  struct cm_array_of_nv_pair prev_nv_pair;
};

/*****************************************************************
  UCM JE Interface with Security Application's callback glue layer
 *****************************************************************/

/*****************************************************************
  UCM JE Configuration Session and Transport Channel
 *****************************************************************/

/******************************************************************************
 * struct cm_je_config_session : This structure is used to hold Configuration Session
 *                        Information by Management Engines.
 * session_id          : Session Identifier
 * mgmt_engine_id        : 
 *                         CM_CLI_MGMT_ENGINE_ID  
 *                         CM_HTTP_MGMT_ENGINE_ID       
 *                         CM_LDSV_MGMT_ENGINE_ID      
 *                         CM_TR069_MGMT_ENGINE_ID     
 *                        Management Engine Identifier from any one of the
 *                        above
 * flags              :  Flags reserved
 * ucAdminRoleLen        : Length of Administrator Role
 * admin_role            : Role of Administrator
 * ucAdminNameLen        : Length of Administrator Name
 * admin_name            : Name of Administrator
 * tnsprt_channel_p    : Pointer to Transport Channel
 *****************************************************************************/

struct cm_je_config_session
{
  uint32_t session_id;
  uint32_t mgmt_engine_id;
  uint32_t flags;
  unsigned char admin_role[CM_MAX_ADMINROLE_LEN +1];   /* Admin role */
  unsigned char admin_name[CM_MAX_ADMINNAME_LEN + 1];   /* Admin role */
  struct cm_tnsprt_channel *tnsprt_channel_p;
};

#endif /* UCMJE_GDEF_H */
