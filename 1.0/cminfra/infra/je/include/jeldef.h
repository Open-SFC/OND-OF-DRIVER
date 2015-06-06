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
 * File name: jeldef.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: 
*/  

#ifndef UCMJE_LDEF_H
#define UCMJE_LDEF_H

/*****************************************************************************
 * * * * * * * * * * *   M A C R O  D E F I N I T I O N S * * * * * * * * * * *
 *****************************************************************************/

/*********************  J E F E A T U R E S ************************************/
#define CM_JE_STATS_SUPPORT
#ifdef CM_JE_DEBUG
//#define CM_JE_LOG_TO_STDOUT
#define CM_JE_LOG_TO_FILE
#endif
/*******************************************************************************/
#define CM_TRACE_ID 1
#define  CM_JE_TRACE_ID 1
#define CM_JE_TRACE_SEVERE  '2'

#define UCMJE_SESSION_BASEID 10000
#define UCMJE_SESSION_MAXID  50000

#define UCMJE_FDPOLL_MAXFDS 32
#define UCMJE_FDPOLL_LEAST_POLL_TIME (2) 

#define CM_JE_RULE_ID_NOT_SET  0x01
#define CM_JE_RULE_ID_SET      0x02

#define JE_CFGREQUEST_MAX_LEN (2 * 1024)
#define  UCMJE_INAC_TIMEOUT  600 /* In Seconds */

#define CM_JE_NO_LOAD_CONFIGURATION "--no-load"

#define DEL_RECORD 1
#define DEL_INSTANCE_ONLY 0
#define EXEC_CMD  1

#ifdef OF_CM_SWITCH_CBK_SUPPORT 
#define  CM_JE_LOG_FILE "/psp/debug/jedebug"
#endif

#ifdef OF_CM_CNTRL_SUPPORT
#define  CM_JE_LOG_FILE "/usr/local/ond_of_driver/1.0/debug/jedebug"
#endif

/* JE Advanced Features*/
#define CM_JE_MAX_REFFERENCE_LEN 100
#define CM_JE_ERROR_STRING_LEN 256
//#define CM_JE_CHECK_MAX_TABLES

#ifndef CM_MGMT_CARD_SUPPORT
//#define CM_TRACE_SUPPORT
#endif

#ifdef CM_JE_LOG_TO_STDOUT /* msgs displayed on stdout as well as stored in file*/
#define CM_JE_DEBUG_PRINT(format,args...) printf("\n%35s(%4d): "format,__FUNCTION__,__LINE__,##args);\
   cm_je_trace( CM_JE_TRACE_ID, CM_JE_TRACE_SEVERE, (char *)__FUNCTION__, __LINE__,format,##args)

#else
#ifdef  CM_JE_LOG_TO_FILE /* Prints redirected to file*/
#define CM_JE_DEBUG_PRINT(format,args...) \
{\
   cm_je_trace( CM_JE_TRACE_ID, CM_JE_TRACE_SEVERE,(char *)__FUNCTION__, __LINE__,format,##args);\
}
#else /* no debug msgs*/
#define CM_JE_DEBUG_PRINT(format,args...)
//printf(format,##args);
#endif
#endif
/*****************************************************************************
 * * * * * * * * * * * * *  E N U M E R A T O R S * * * * * * * * * * * * * * *
 *****************************************************************************/
typedef enum
{
   UCMJE_CFGSESS_ACTIVE_STATE,
   UCMJE_MAX_STATE
} UCMJEFSMState_e;

typedef enum
{
   UCMJE_UPDATE_EVENT,
   UCMJE_COMMIT_EVENT,
   UCMJE_REVOKE_EVENT,
   UCMJE_TIMEOUT_EVENT,
   UCMJE_GET_EVENT,
   UCMJE_MAX_EVENT
} UCMJEFSMEvent_e;

typedef enum
{
   JE_CFGREQUEST_FIRST_MESSAGE = 10,
   JE_CFGREQUEST_INCOMPLETE_MESSAGE,
   JE_CFGREQUEST_COMPLETE_MESSAGE,
   JE_CFGREQUEST_NEW_MESSAGE,
} UCMJEMsgStatus_e;

enum
{
   JE_CFGREQ_RECEIVES=0,
   JE_RESPONSE_SENDS,
   JE_TRANSCHANNEL_STARTED,
   JE_TRANSCHANNEL_CLOSED,
   JE_CFGSESSION_STARTED,
   JE_CFGSESSION_CLOSED,
   JE_CFGSESSION_COMMITTED,
   JE_CFGSESSION_REVOKED,
   JE_CFGSESSION_COMMITFAILED,
   JE_CFGSESSIONREQ_RECIEVES,
   JE_DMREQ_RECIEVES,
   JE_LDSVREQ_RECIEVES,
   JE_SECAPPLREQ_RECIEVES,
   JE_VERHISTORYREQ_RECIEVES,
   JE_NOTIFICATION_RECIEVES,
   JE_STATS_MAX
};


enum
{
   /* Configuration specific statistics */
   JE_SESSION_ADD_PARAMS = 0,
   JE_SESSION_SET_PARAMS,
   JE_SESSION_DEL_PARAMS,
   JE_SESSION_CANCEL_PREV_CMD,
   JE_SESSION_COMPARE_PARAM,        /*5 */
   JE_SESSION_GET_CONFIG_SESSION,  
   JE_SESSION_SET_PARAM_TO_DEFAULT,
   JE_SESSION_SET_DEFAULTS,
   JE_SESSION_STATS_MAX
};

enum
{
   /* Security Application specific statistics */
   JE_SECAPPL_GET_FIRST_N_RECS = 0,
   JE_SECAPPL_GET_NEXT_N_RECS,
   JE_SECAPPL_GET_EXACT_REC,
   JE_SECAPPL_GET_REQ_COUNT,
   JE_SECAPPL_GET_REQ_FAILED,
   JE_SECAPPL_STATS_MAX
};

#define CM_JE_NUMBER_OF_REQUESTS 100

#ifdef CM_JE_STATS_SUPPORT
#define CM_INIT_JE_STATS of_memset(cm_je_global_stats_g,0,JE_STATS_MAX * sizeof(int32_t))
#define CM_INIT_JESESSION_STATS of_memset(  cm_je_session_stats_g,0,\
      JE_SESSION_STATS_MAX * sizeof(int32_t))
#define CM_INIT_JESECAPPL_STATS of_memset(  cm_je_appl_stats_g,0,\
      JE_SECAPPL_STATS_MAX * sizeof(int32_t))

#define CM_INC_STATS_JE(field) \
{\
   if ( field >= JE_STATS_MAX)\
   {\
      CM_JE_DEBUG_PRINT("Unable to increment Stats for given Index");\
   }\
   CM_INC_STATS(cm_je_global_stats_g,field);\
}

#define CM_DEC_STATS_JE(field) \
{\
   if ( field >= JE_STATS_MAX)\
   {\
      CM_JE_DEBUG_PRINT("Unable to Decrement Stats for given Index");\
   }\
   CM_DEC_STATS(cm_je_global_stats_g,field);\
}

#define CM_JE_PRINT_STATS \
{\
   uiStastsCnt++;\
   if ((uiStastsCnt % CM_JE_NUMBER_OF_REQUESTS) == 0 )\
   {\
      CM_JE_PRINT_GLOBAL_STATS;\
      CM_JE_PRINT_SESSON_STATS;\
      CM_JE_PRINT_SECAPPL_STATS;\
   }\
}

#define CM_JE_PRINT_GLOBAL_STATS \
{\
   CM_JE_DEBUG_PRINT("***JE STATISTICS***");\
   CM_JE_DEBUG_PRINT("CfgRequest_Received=%d RESPONSE_SENDS=%d \
         TransportChannel_Started=%d TransportChannel_Closed=%d CfgSession_Started=%d\
         CfgSession_Closed=%d CfgSession_Commited=%d   CfgSession_Revoked=%d\
         CfgSessionREQ_Received=%d DMREQ_Received=%d LdsvREQ_Received=%d \
         sec_applREQ_Received=%d",\
         cm_je_global_stats_g[JE_CFGREQ_RECEIVES],\
         cm_je_global_stats_g[JE_RESPONSE_SENDS],\
         cm_je_global_stats_g[JE_TRANSCHANNEL_STARTED],\
         cm_je_global_stats_g[JE_TRANSCHANNEL_CLOSED],\
         cm_je_global_stats_g[JE_CFGSESSION_STARTED],\
         cm_je_global_stats_g[JE_CFGSESSION_CLOSED],\
         cm_je_global_stats_g[JE_CFGSESSION_COMMITTED],\
         cm_je_global_stats_g[JE_CFGSESSION_REVOKED],\
         cm_je_global_stats_g[JE_CFGSESSIONREQ_RECIEVES],\
         cm_je_global_stats_g[JE_DMREQ_RECIEVES],\
         cm_je_global_stats_g[JE_LDSVREQ_RECIEVES],\
         cm_je_global_stats_g[JE_SECAPPLREQ_RECIEVES]);\
}


#define CM_INC_STATS_JESESSION(field) \
{\
   if ( field >= JE_SESSION_STATS_MAX)\
   {\
      CM_JE_DEBUG_PRINT("Unable to increment Stats for given Index");\
   }\
   CM_INC_STATS(cm_je_session_stats_g,field);\
}

#define CM_DEC_STATS_JESESSION(field) \
{\
   if ( field >= JE_SESSION_STATS_MAX)\
   {\
      CM_JE_DEBUG_PRINT("Unable to Decrement Stats for given Index");\
   }\
   CM_DEC_STATS(cm_je_session_stats_g,field);\
}

#define CM_JE_PRINT_SESSON_STATS \
{\
   CM_JE_DEBUG_PRINT("***JE Session STATISTICS***");\
   CM_JE_DEBUG_PRINT("Add=%d Set=%d  Delete=%d CancelPrevCmd=%d \
         Compare=%d GetCfgSession=%d SetParamDefault=%d SetDefault=%d",\
         cm_je_session_stats_g[JE_SESSION_ADD_PARAMS],\
         cm_je_session_stats_g[JE_SESSION_SET_PARAMS],\
         cm_je_session_stats_g[JE_SESSION_DEL_PARAMS],\
         cm_je_session_stats_g[JE_SESSION_CANCEL_PREV_CMD],\
         cm_je_session_stats_g[JE_SESSION_COMPARE_PARAM],\
         cm_je_session_stats_g[JE_SESSION_GET_CONFIG_SESSION],\
         cm_je_session_stats_g[JE_SESSION_SET_PARAM_TO_DEFAULT],\
         cm_je_session_stats_g[JE_SESSION_SET_DEFAULTS]);\
}


#define CM_INC_STATS_JESECAPPL(field) \
{\
   if ( field >= JE_SECAPPL_STATS_MAX)\
   {\
      CM_JE_DEBUG_PRINT("Unable to increment Stats for given Index");\
   }\
   CM_INC_STATS(cm_je_appl_stats_g,field);\
}

#define CM_DEC_STATS_JESECAPPL(field) \
{\
   if ( field >= JE_SECAPPL_STATS_MAX)\
   {\
      CM_JE_DEBUG_PRINT("Unable to Decrement Stats for given Index");\
   }\
   CM_DEC_STATS(cm_je_appl_stats_g,field);\
}


#define CM_JE_PRINT_SECAPPL_STATS \
{\
   CM_JE_DEBUG_PRINT("***JE Security Application STATISTICS***");\
   CM_JE_DEBUG_PRINT("GetFirst=%d GetNext=%d  GetExact=%d \
         Total=%d Failed=%d",\
         cm_je_appl_stats_g[JE_SECAPPL_GET_FIRST_N_RECS],\
         cm_je_appl_stats_g[JE_SECAPPL_GET_NEXT_N_RECS],\
         cm_je_appl_stats_g[JE_SECAPPL_GET_EXACT_REC],\
         cm_je_appl_stats_g[JE_SECAPPL_GET_REQ_COUNT],\
         cm_je_appl_stats_g[JE_SECAPPL_GET_REQ_FAILED]);\
}

#else
#define CM_INIT_JE_STATS
#define CM_INIT_JESESSION_STATS
#define CM_INIT_JESECAPPL_STATS 
#define CM_INC_STATS_JE(field) 
#define CM_DEC_STATS_JE(field)
#define CM_JE_PRINT_STATS
#define CM_JE_PRINT_GLOBAL_STATS 
#define CM_INC_STATS_JESESSION(field)
#define CM_DEC_STATS_JESESSION(field)
#define CM_JE_PRINT_SESSON_STATS
#define CM_INC_STATS_JESECAPPL(field)
#define CM_DEC_STATS_JESECAPPL(field)
#define CM_JE_PRINT_SECAPPL_STATS
#endif


#define CM_JE_PRINT_IVPAIR_ARRAY(arr_iv_pair_p)\
   if(arr_iv_pair_p)\
{\
   uint32_t count_ui;\
   for(count_ui=0;count_ui < arr_iv_pair_p->count_ui; count_ui++)\
   {\
      CM_JE_PRINT_IVPAIR(arr_iv_pair_p->iv_pairs[count_ui]);\
   }\
}
#define CM_JE_PRINT_NVPAIR_ARRAY(arr_nv_pair_p)\
   if(arr_nv_pair_p)\
{\
   uint32_t count_ui;\
   for(count_ui=0;count_ui < arr_nv_pair_p->count_ui; count_ui++)\
   {\
      CM_JE_PRINT_NVPAIR(arr_nv_pair_p->nv_pairs[count_ui]);\
   }\
}

/****************************************************************************
 * * * * * * * * S T R U C T U R E   D E F I N I T I O N S* * * * * * * * * * 
 *****************************************************************************/
/* * * * * CONFIGURATION SESSION * * * * * * */
 struct je_config_session_cache
{
   uint32_t no_of_commands;
   UCMDllQ_t add_config_list;
   UCMDllQ_t modify_config_list;
   UCMDllQ_t delete_config_list;
   UCMDllQ_t xtn_config_list;
   unsigned char heap_b;
};

struct je_config_session
{
   struct je_config_session_cache *pCache;
   uint32_t session_id;
   uint32_t mgmt_engine_id;
   unsigned char admin_role[CM_MAX_ADMINROLE_LEN + 1];
   unsigned char admin_name[CM_MAX_ADMINNAME_LEN + 1];
   void *tnsprt_channel_p;
   int32_t state;
   //  int32_t iEvent;
   uint32_t flags;
   unsigned char heap_b;
};

 struct je_command_list
{
   UCMDllQNode_t list_node;
   uint32_t sequence_id;
   unsigned char referrence_in_cache_b;
   unsigned char success_b;
   struct cm_command *command_p;  
   unsigned char xtn_b;
   unsigned char exec_command_b;
   unsigned char del_instance_tree_b;
   uint32_t state;
} ;

struct je_xtn_entry
{
   UCMDllQNode_t list_node;
   unsigned char status_b;
   char *pcDMPath; 
   void *xtn_rec;
   UCMDllQ_t CmdList;
};


struct cmje_tnsprtchannel_info
{
   UCMDllQNode_t list_node;
   uint32_t max_inactive_time;
   //JEArrayofAttirbuteIvPairs_t  JEAttributes;
   cm_timer_t *session_timer_p;
   char *recv_buf_p;
   /* currently JE is not supporting partial messages.
    * If it supports partial message following members will be used*/
   // int32_t iTotInputLen;
   //int32_t iBytesRecd;
   //int32_t iBytesToRead;
   //int32_t iReqStatus;
   cm_sock_handle sock_fd_ui;
   struct je_config_session *config_session_p;
   unsigned char heap_b;
  cm_sock_handle child_sock_fd_ui;
};

struct je_command_request
{
   uint32_t command_id;
   uint32_t sub_command_id;            /* add/set/del/getfirst/getnext, etc */
   char *dm_path_p;             /*Data Model Path string */
   struct cm_array_of_nv_pair nv_pairs;      /* array of name-value pairs */
};

struct je_config_request
{
   uint32_t command_id;           /*(start_config/stop_config/update_cmd/command_response) */
   uint32_t flags;             /* reserved: future purpose */
   uint32_t sequence_id;            /* reserved: future purpose */
   uint32_t mgmt_engine_id;      /* sender identification. For eg: mgmt engine id */
   unsigned char admin_role[CM_MAX_ADMINROLE_LEN+1];
   unsigned char admin_name[CM_MAX_ADMINNAME_LEN+1];

   uint32_t length;            /* Total message length */
   uint32_t sub_command_id;            /* add/set/del/getfirst/getnext, etc */
   unsigned char *dm_path_p;            /*Data Model Path string */
   union
   {
      struct cm_array_of_nv_pair nv_pair_array;    /* array of name-value pairs */
      struct
      {
         uint32_t count_ui;
         struct cm_array_of_nv_pair key_nv_pairs;       /* array of name-value pairs */
         struct cm_array_of_nv_pair prev_rec_key_nv_pairs;       /* array of name-value pairs */
      } sec_appl;
      struct
      {
         uint32_t type;
#if UCMDM_INSTANCE_ROLE_AND_PERMISSIONS_USING_KEY
         void *key_p;
#endif
         uint32_t opaque_info_length;
         void *opaque_info_p;
      } data_model;
      struct
      {
         uint64_t version;  
         uint64_t end_version;  
      }version;
      struct
      {
         uint32_t event;
         void *pData;
      }Notify;
#ifdef CM_STATS_COLLECTOR_SUPPORT
     struct
     {
       int32_t result_code;
       uint32_t nv_pair_flat_buf_len_ui;
       void *nv_pair_flat_buf_p;
     }stats_data; 
#endif
   } data;
   unsigned char heap_b;
};

struct je_request_node
{
   UCMDllQNode_t list_node;
	 cm_sock_handle fd_ui;
	 struct je_config_request *config_request_p;
	 struct cm_result *result_p;
};

struct je_stack_node
{   
   struct cm_dm_data_element *pDMNode;
   UCMDM_Inode_t *pDMInode;
   char *dm_path_p;
   void *opaque_info_p;
   uint32_t opaque_info_length;
   struct cm_array_of_iv_pairs *pIvPairArr;
   UCMDllQNode_t list_node;
};




/*****************************************************************************
 * * * * *  F U N C T I O N  P O I N T E R  P R O T O T Y P E * * * * * * * * 
 *****************************************************************************/
typedef int32_t (*JESMFuncPtr) (struct je_config_session *, struct je_config_request *,
      struct cm_result *);

/*****************************************************************************
 * * * * * * * * * * *  U T I L I T Y  M A C R O S * * * * * * * * * * * * * 
 *****************************************************************************/

/*Freeing NV PAIR*/
#define UCMJE_FREE_NVPAIR(NvPair)  CM_FREE_NVPAIR(NvPair)

#define UCMJE_FREE_PTR_NVPAIR(pNvPair)\
   CM_FREE_PTR_NVPAIR(pNvPair)

#define UCMJE_FREE_PTR_NVPAIR_MEMBEROF_ARRAY(pNvPair)\
   CM_FREE_PTR_NVPAIR_MEMBEROF_ARRAY(pNvPair)

#define UCMJE_FREE_NVPAIR_ARRAY(nv_pair_array,count_ui) \
   CM_FREE_NVPAIR_ARRAY(nv_pair_array,count_ui)

#define UCMJE_FREE_PTR_NVPAIR_ARRAY(pnv_pair_array,count_ui)\
   CM_FREE_PTR_NVPAIR_ARRAY(pnv_pair_array,count_ui)

/*Freeing IV PAIR*/
#define UCMJE_FREE_IVPAIR(IvPair)  CM_FREE_IVPAIR(IvPair)

#define UCMJE_FREE_PTR_IVPAIR(pIvPair)\
   CM_FREE_PTR_IVPAIR(pIvPair)

#define UCMJE_FREE_PTR_IVPAIR_MEMBEROF_ARRAY(pIvPair)\
   CM_FREE_PTR_IVPAIR_MEMBEROF_ARRAY(pIvPair)

#define UCMJE_FREE_IVPAIR_ARRAY(IvPairArray,count_ui) \
   CM_FREE_IVPAIR_ARRAY(IvPairArray,count_ui)

#define UCMJE_FREE_IVPAIR_PTR(pIvPairArray,count_ui) \
   CM_FREE_IVPAIR_PTR(pIvPairArray,count_ui)

#define UCMJE_FREE_PTR_IVPAIR_ARRAY(pIvPairArray,count_ui)\
   CM_FREE_PTR_IVPAIR_ARRAY(pIvPairArray,count_ui)

/*Freeing COMMAND*/
#define UCMJE_FREE_COMMAND(command_p) \
{\
   if(command_p)\
   {\
      UCMJE_FREE_NVPAIR_ARRAY(command_p->nv_pair_array, command_p->nv_pair_array.count_ui);\
      if(command_p->dm_path_p)\
      {\
         of_free(command_p->dm_path_p);\
         command_p->dm_path_p=NULL;\
      }\
      of_free(command_p);\
      command_p=NULL;\
   }\
}

/*Freeing COMMAND List*/
#define UCMJE_FREE_COMMANDLIST(pCmdList) \
{\
   if(pCmdList)\
   {\
      UCMJE_FREE_COMMAND(pCmdList->command_p);\
      of_free(pCmdList);\
      pCmdList=NULL;\
   }\
}

#define UCMJE_FREE_ARRAY_OF_COMMANDS(array_of_commands_p, count_ui) \
{\
   uint32_t uindex_i; \
   for (uindex_i=0; uindex_i < count_ui; uindex_i++)\
   {\
      of_free(array_of_commands_p->Commands[uindex_i].dm_path_p);\
      UCMJE_FREE_NVPAIR_ARRAY(array_of_commands_p->Commands[uindex_i].nv_pair_array,\
            array_of_commands_p->Commands[uindex_i].nv_pair_array.count_ui);\
   }\
   of_free(array_of_commands_p->Commands);\
   of_free(array_of_commands_p);\
}

#define UCMJE_COPY_NVPAIR_TO_NVPAIR(InNvPair,OutNvPair) \
{\
   /* Copy Name Length */\
   OutNvPair.name_length = InNvPair.name_length;\
   /* Allocate Memory for Name */\
   OutNvPair.name_p = (char *)of_calloc (1, OutNvPair.name_length+1);\
   if (!(OutNvPair.name_p))\
   {\
      CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Name Field");\
      return OF_FAILURE;\
   }\
   /* Copy Name */\
   of_strncpy (OutNvPair.name_p, InNvPair.name_p, OutNvPair.name_length);\
   /* Copy Value type */ \
   OutNvPair.value_type = InNvPair.value_type;\
   /* Copy Value Length */\
   OutNvPair.value_length = InNvPair.value_length;\
   /* Allocate Memory for Value */\
   OutNvPair.value_p =(char *) of_calloc (1, OutNvPair.value_length+1);\
   if (!(OutNvPair.value_p))\
   {\
      CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Value Field");\
      return OF_FAILURE;\
   }\
   /* Copy Value */\
   of_strncpy (OutNvPair.value_p, InNvPair.value_p, OutNvPair.value_length);\
   /* CM_JE_PRINT_NVPAIR(OutNvPair);*/\
}

#define UCMJE_COPY_IVPAIR_TO_IVPAIR(InIvPair,OutIvPair) \
{\
   /* Copy Id */\
   OutIvPair.id_ui =InIvPair.id_ui;\
   /* Copy Value type */ \
   OutIvPair.value_type = InIvPair.value_type;\
   /* Copy Value Length */\
   OutIvPair.value_length = InIvPair.value_length;\
   /* Allocate Memory for Value */\
   OutIvPair.value_p =(char *) of_calloc (1, OutIvPair.value_length+1);\
   if (!(OutIvPair.value_p))\
   {\
      CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Value Field");\
      return OF_FAILURE;\
   }\
   /* Copy Value */\
   of_strncpy (OutIvPair.value_p, InIvPair.value_p, OutIvPair.value_length);\
}

#define UCMJE_COPY_PTR_NVPAIR_TO_NVPAIR(pInNvPair,OutNvPair) \
{\
   /* Copy Id */\
   OutNvPair.name_length =pInNvPair->name_length;\
   /* Copy Name */\
   OutNvPair.name_p = (char *)of_calloc(1,OutNvPair.name_length +1);\
   if (!OutNvPair.name_p)\
   {\
      CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Value Field");\
      return OF_FAILURE;\
   }\
   of_strncpy (OutNvPair.name_p, pInNvPair->name_p, OutNvPair.name_length);\
   /* Copy Value type */ \
   OutNvPair.value_type = pInNvPair->value_type;\
   /* Copy Value Length */\
   OutNvPair.value_length = pInNvPair->value_length;\
   /* Allocate Memory for Value */\
   OutNvPair.value_p =(char *) of_calloc (1, OutNvPair.value_length+1);\
   if (!(OutNvPair.value_p))\
   {\
      CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Value Field");\
      return OF_FAILURE;\
   }\
   /* Copy Value */\
   of_strncpy (OutNvPair.value_p, pInNvPair->value_p, OutNvPair.value_length);\
}

#define UCMJE_MEMALLOC_AND_COPY_ERROR_DMPATH_INTO_UCMRESULT(result_p,dm_path_p)\
{\
   if(dm_path_p)\
   {\
      result_p->result.error.dm_path_p=of_calloc(1,of_strlen(dm_path_p)+1);\
      if(result_p->result.error.dm_path_p)\
      {\
         of_strncpy( result_p->result.error.dm_path_p,dm_path_p,of_strlen( dm_path_p));\
      }\
      else\
      {\
         CM_JE_DEBUG_PRINT("Memory Allocation Failed for Error DMPath");\
      }\
   }\
   else\
   {\
      CM_JE_DEBUG_PRINT("DM Path is NULL");\
   }\
}

#define UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT(result_p,iErrorCode,dm_path_p)\
{\
   if(result_p)\
   {\
      result_p->result.error.error_code=iErrorCode;\
      if (iErrorCode != UCMJE_ERROR_DMPATH_NULL)\
      UCMJE_MEMALLOC_AND_COPY_ERROR_DMPATH_INTO_UCMRESULT(result_p,dm_path_p);\
   }\
   else{\
      CM_JE_DEBUG_PRINT("Result Structure is NULL");\
   }\
}

#define UCMJE_MEMALLOC_AND_COPY_RESULTSTRING_INTO_UCMRESULT(result_p,String)\
{\
   uint32_t uiStrLen=0;\
   if(result_p)\
   {\
      if(String)\
      {\
         uiStrLen=of_strlen(String);\
         result_p->result.error.result_string_p = (char *) of_calloc\
         (1,uiStrLen+1);\
         if(result_p->result.error.result_string_p)\
         {\
            of_strncpy(result_p->result.error.result_string_p,String,uiStrLen);\
         }\
      }\
   }\
   else{\
      CM_JE_DEBUG_PRINT("Result Structure is NULL");\
   }\
}

#define CM_JE_PRINT_NVPAIR(pNvPair) \
   if(pNvPair.value_p)\
{\
   CM_JE_DEBUG_PRINT ("Len=%d Name=%s ValType=%d Len=%d Value=%s",\
         pNvPair.name_length,\
         pNvPair.name_p,\
         pNvPair.value_type,\
         pNvPair.value_length,(char *)pNvPair.value_p);\
}\
else\
{\
   CM_JE_DEBUG_PRINT ("Len=%d Name=%s ValType=%d Len=%d ",\
         pNvPair.name_length,\
         pNvPair.name_p,\
         pNvPair.value_type,\
         pNvPair.value_length);\
}


//#define CM_JE_PRINT_PTR_NVPAIR(pNvPair) CM_PRINT_PTR_NVPAIR(pNvPair)
#define CM_JE_PRINT_PTR_NVPAIR(pNvPair) \
   if(pNvPair->value_p)\
{\
   CM_JE_DEBUG_PRINT ("Len=%d Name=%s ValType=%d Len=%d Value=%s",\
         pNvPair->name_length,\
         pNvPair->name_p,\
         pNvPair->value_type,\
         pNvPair->value_length,(char *)pNvPair->value_p);\
}\
else\
{\
   CM_JE_DEBUG_PRINT ("Len=%d Name=%s ValType=%d Len=%d ",\
         pNvPair->name_length,\
         pNvPair->name_p,\
         pNvPair->value_type,\
         pNvPair->value_length);\
}

//#define CM_JE_PRINT_IVPAIR(pIvPair) CM_PRINT_IVPAIR(pIvPair)
#define CM_JE_PRINT_IVPAIR(pIvPair) \
  CM_JE_DEBUG_PRINT ("id_ui=%2d ValType=%2d Len=%3d Value=%s",\
         pIvPair.id_ui,   pIvPair.value_type,\
         pIvPair.value_length, (char *) pIvPair.value_p)

//#define CM_JE_PRINT_PTR_IVPAIR(pIvPair) CM_PRINT_PTR_IVPAIR(pIvPair)
#define CM_JE_PRINT_PTR_IVPAIR(pIvPair) \
if (pIvPair)\
{\
	 CM_JE_DEBUG_PRINT ("id_ui=%2d ValType=%2d Len=%3d Value=%s",\
				 pIvPair->id_ui,   pIvPair->value_type,\
				 pIvPair->value_length, (char *) pIvPair->value_p);\
}\


#define UCMJE_ELEMENT_NAME_FROM_REFERRENCE(aComplePath, pElementName)\
{\
   char  *sep = ".";\
   char *word;\
   \
   for (word = strtok(aComplePath, sep);\
         word;\
         word = strtok(NULL, sep))\
   {\
      {\
         pElementName=word;\
      }\
   }\
}

#define CM_JE_COMPARE_KEY_NVPAIR_NAME(NvPair1, NvPair2)\
   (!strcmp(NvPair1.name_p, NvPair2.name_p))

#define CM_JE_COMPARE_KEY_NVPAIR_VALUE(NvPair1, NvPair2)\
   (!strcmp(NvPair1.value_p, NvPair2.value_p))

#define CM_JE_COMPARE_KEY_NVPAIR(NvPair1, NvPair2)\
   ((NvPair2.value_length > 0 )?\
    (CM_JE_COMPARE_KEY_NVPAIR_NAME(NvPair1, NvPair2) &&\
     CM_JE_COMPARE_KEY_NVPAIR_VALUE(NvPair1, NvPair2)): FALSE)

#define CM_JE_GET_NVPAIR_LEN(NvPair, uiLen) \
{\
   uiLen += CM_UINT32_SIZE; \
   uiLen += NvPair.name_length; \
   uiLen += CM_UINT32_SIZE;\
   uiLen += CM_UINT32_SIZE;\
   uiLen += NvPair.value_length;\
}

#ifdef  CM_JE_DEBUG
#define CM_JE_PRINT_ARRAY_OF_COMMANDS(array_of_commands_p) \
{\
   uint32_t uindex_i; \
   struct cm_array_of_nv_pair *arr_nv_pair_p;\
   uint32_t count_ui; \
   if(array_of_commands_p)\
   {\
      for (uindex_i=0; uindex_i < array_of_commands_p->count_ui; uindex_i++)\
      {\
         CM_JE_DEBUG_PRINT("DMPath=%s",array_of_commands_p->Commands[uindex_i].dm_path_p);\
         CM_JE_DEBUG_PRINT("Command Id=%d",array_of_commands_p->Commands[uindex_i].command_id);\
         arr_nv_pair_p=&array_of_commands_p->Commands[uindex_i].nv_pair_array;\
         for(count_ui=0;count_ui < arr_nv_pair_p->count_ui; count_ui++)\
         {\
            CM_JE_PRINT_NVPAIR(arr_nv_pair_p->nv_pairs[count_ui]);\
         }\
      }\
   }\
}

#define CM_JE_PRINT_COMMAND(command_p) \
{\
   uint32_t count_ui; \
   struct cm_array_of_nv_pair *arr_nv_pair_p;\
   if(command_p)\
   {\
      CM_DEBUG_PRINT("DMPath=%s",command_p->dm_path_p);\
      CM_DEBUG_PRINT("Command Id=%d",command_p->command_id);\
      arr_nv_pair_p=&command_p->nv_pair_array;\
      for(count_ui=0;count_ui < arr_nv_pair_p->count_ui; count_ui++)\
      {\
         CM_JE_PRINT_NVPAIR(arr_nv_pair_p->nv_pairs[count_ui]);\
      }\
   }\
}
#else
#define CM_JE_PRINT_ARRAY_OF_COMMANDS(array_of_commands_p) 
#define CM_JE_PRINT_COMMAND(command_p) 
//#define CM_JE_PRINT_PTR_NVPAIR(pNvPair) 
#endif
#endif
