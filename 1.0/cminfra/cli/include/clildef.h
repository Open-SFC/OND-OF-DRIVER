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
 * File name:  clildef.h
 * Author: Freescale Semiconductor    
 * Date:   01/20/2013
 * Description: 
*/

#ifndef CLI_LDEF_H
#define CLI_LDEF_H

#ifdef CM_CLI_DEBUG
#define UCMCLIDEBUG printf
#else
#define UCMCLIDEBUG(format,args...)
#endif

#define CM_CLI_ENUM_SEPERATOR "/"
#define CM_CLI_MAX_SESSIONS     1
#define CM_CLI_MAX_PROMPT_LEN   1024
#define CM_CLI_MAX_NAME_LEN     512
#define CM_CLI_MAX_INSTALL_PATH 1024
#define CM_CLI_MAX_DISPLAY_LEN  512
#define CM_CLI_DEFAULT_DIR_LENGTH 512
#define CM_CLI_DEFAULT_ARRAY_LENGTH 512
#define CM_CLI_MAX_HISTORY_LINES 64
#define CM_CLI_INACTIVITY_TIMEOUT 600
#define CM_CLI_MAX_DEPTH  15
#define CM_CLI_AGGRREGATE_STATS "aggr"
#define CM_CLI_AVERAGE_STATS "avg"
#define CM_CLI_PER_DEVICE_STATS "perdev"

#define SIGACTION struct sigaction

#define CM_CLI_STDIN stdin
struct cm_array_of_structs *table_info_arr_p_g; 
char table_name_a_g[32];

enum tok_stat
{
  EOLN = 0,
  MOREINPUT
};

struct cm_cli_data_ele_node
{
  UCMDllQNode_t list_node;
  unsigned char node_name[CM_CLI_MAX_NAME_LEN + 1];
  char *friendly_name_p;
  char data_type;
  char *description_p;
  char node_type;
  unsigned char system_command_b;
  unsigned char non_config_leaf_node_b;
  unsigned char mandatory_b;
  unsigned char key_b;
  unsigned char heap_b;
  unsigned char parent_trans_b;
  unsigned char visible_b;
  char *command_name_p;
  char *cli_identifier_p;
  char *cli_idenetifier_help_p;  
#if 0
  struct
  {
    unsigned char data_type:4;      /* int/unsigned int/string/boolean/base64 */
    unsigned char attrib_type:4;    /* range/enum/etc */
    union
    {
      struct
      {
        int32_t start_value;
        int32_t end_value;
      } int_range;
      struct
      {
        uint32_t start_value;
        uint32_t end_value;
      } uint_range;
      struct
      {
        int64_t start_value;
        int64_t end_value;
      } longint_range;
      struct
      {
        uint64_t start_value;
        uint64_t end_value;
      } ulongint_range;
      struct
      {
        uint32_t min_length;
        uint32_t max_length;
      } string_range;
      struct
      {
        char start_ip[16];
        char end_ip[16];
      } ip_range;
      struct
      {
        uint32_t count_ui;
        int32_t array[5];
      } int_enum;
      struct
      {
        uint32_t count_ui;
        uint32_t array[5];
      } uint_enum;
      struct
      {
        uint32_t count_ui;
        int64_t array[5];
      } longint_enum;
      struct
      {
        uint32_t count_ui;
        uint64_t array[5];
      } ulongint_enum;
      struct
      {
        uint32_t count_ui;
        char *array[CM_MAX_STRENUM_COUNT];
        char *aFrdArr[CM_MAX_STRENUM_COUNT];
      } string_enum;
      struct
      {
        uint32_t count_ui;
        unsigned char ip_arr_a[5][16];
      } ip_enum;
      struct
      {
        uint32_t count_ui;
        unsigned char ip_v6_arr_a[5][128];
      } ipv6_enum;
      struct
      {
        char start_ip[16];
        char end_ip[16];
      } ipv6_range;
      uint32_t max_no_of_tables;
    } Attr;
    union
    {
      uint64_t uldefault_value;
      int64_t ldefault_value;
      uint32_t uidefault_value;
      int32_t idefault_value;
      char default_string[50];
    } default_value;
  } data_attrib;
#endif
   struct cm_dm_data_attrib data_attrib;
};

struct cm_cli_node
{
  UCMDllQ_t child_list;
  UCMDllQ_t data_elements_list;
  struct cm_cli_data_ele_node key_node_info;
  unsigned char node_name[CM_CLI_MAX_NAME_LEN + 1];
  unsigned char install_path[CM_CLI_MAX_INSTALL_PATH + 1];
  uint8_t node_type_ui;
  unsigned char heap_b;
  unsigned char parent_trans_b;
  unsigned char global_trans_b;
  unsigned char scalar_group_b;
  unsigned char non_config_leaf_node_b;
};

struct cm_cli_nvpair_data
{  
   uint32_t friendly_name_length;
   char *friendly_name_p;
   uint32_t enum_val_len_ui;
   void *enum_value_p;
   uint32_t identifier_len_ui;     /* Optional */
   char *cli_identifier_p;  /* Optional */
};

/* Cli Session data structure */
struct cm_cli_session
{
  char root_c[CM_CLI_MAX_NAME_LEN + 1];
  char current_path[CM_CLI_MAX_INSTALL_PATH + 1];
  char context_path[CM_CLI_MAX_INSTALL_PATH + 1];
  char dmpath_a[CM_DM_MAX_PATH_LEN + 1];
  char prompt[CM_CLI_MAX_DEPTH + 1][CM_CLI_MAX_NAME_LEN + 1];
  uint8_t index_i;
  unsigned char context_b;
  struct cm_array_of_strings *string_arr_p;
  struct cm_role_info role_info;
  struct cm_cli_node *current_node_p;
  void *config_session_p;
  void *tnsprt_channel_p;

  /* User details */
  uint32_t inact_time_out;


  edit_line *edit_line_ctx_p;             /* Edit line context */
  history_t *history_p;            /* history_t  context */
  hist_event hist_event;          /* history_t of event */
  
/* cd details */
  unsigned char cd_table_b;
  uint32_t no_of_cds;
  unsigned char curr_table_ctx_b;

  struct cm_cli_session *next_p;
};

#define CM_MAX_ATTRIB_NAMELEN   31
#define CM_MAX_ATTRIB_VALUELEN   255

struct cm_cmd_attribs
{
  char attrib_name_a[CM_MAX_ATTRIB_NAMELEN + 1];
  char attrib_value_a[CM_MAX_ATTRIB_VALUELEN + 1];
};

#define UCMCLI_COPY_NVPAIR_TO_NVPAIR(in_nv_pair,out_nv_pair) \
{\
   /* Copy Name Length */\
   out_nv_pair.name_length = in_nv_pair.name_length;\
   /* Allocate Memory for Name */\
   out_nv_pair.name_p = (char *)of_calloc (1, out_nv_pair.name_length+1);\
   if (!(out_nv_pair.name_p))\
   {\
      UCMCLIDEBUG ("Memory Allocation Failed for Name Field");\
         return OF_FAILURE;\
   }\
   /* Copy Name */\
      of_strncpy (out_nv_pair.name_p, in_nv_pair.name_p, out_nv_pair.name_length);\
      /* Copy Value type */ \
      out_nv_pair.value_type = in_nv_pair.value_type;\
      /* Copy Value Length */\
      out_nv_pair.value_length = in_nv_pair.value_length;\
      /* Allocate Memory for Value */\
      out_nv_pair.value_p =(char *) of_calloc (1, out_nv_pair.value_length+1);\
      if (!(out_nv_pair.value_p))\
      {\
         UCMCLIDEBUG ("Memory Allocation Failed for Value Field");\
            return OF_FAILURE;\
      }\
   /* Copy Value */\
      of_strncpy (out_nv_pair.value_p, in_nv_pair.value_p, out_nv_pair.value_length);\
}
#endif
