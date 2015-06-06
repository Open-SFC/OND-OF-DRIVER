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
 * File name: dmgdef.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/26/2013
 * Description: This file contains data element definition for  
 *              all possible data types.
 * 
 */

#ifndef UCMDM_GDEF_H
#define UCMDM_GDEF_H

#define CM_DM_MAX_PATH_LEN                   1024
#define UCMDM_ROOT_NODE_ID                   0



#define UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM   0  /* pre-order traversal */
#define UCMDM_TRAVERSAL_TYPE_BOTTOM_TO_TOP   1  /* post-order traversal */

#ifdef CM_MGMT_CARD_SUPPORT
#define CM_DM_ROOT_NODE_NAME "mgmt"
#define CM_DM_ROOT_NODE_PATH "mgmt"
#else
#ifdef OF_CM_CNTRL_SUPPORT
#define CM_DM_ROOT_NODE_NAME "on_director"
#define CM_DM_ROOT_NODE_PATH "on_director"
#else
#define CM_DM_ROOT_NODE_NAME "on_switch"
#define CM_DM_ROOT_NODE_PATH "on_switch"
#endif
#endif

#define CM_DM_ELEMENT_RELATIVITY_NAME "relativity"
#define CM_DM_ELEMENT_RELATIVE_KEY_NAME "relativekey"
#define CM_DM_ELEMENT_BPASSIV_KEY_NAME "bPassive"
#define CM_DM_ELEMENT_RELATIVITY_NAME_LEN 10
#define CM_DM_ELEMENT_RELATIVITY_KEY_LEN 11

#define CM_DM_ELEMENT_RELATIVITY_ID  10000
#define CM_DM_ELEMENT_RELATIVE_KEY_ID 10001

#define CM_DM_MAX_KEY_INSTANCES 5
#define CM_DM_MAX_FRDNAME_SIZE 128

enum cm_chanid
{
  CM_SELF_CHANNEL_ID=0,
  CM_CP_CHANNEL_ID,
  CM_DP_CHANNEL_ID,
  CM_CPDP_CHANNEL_ID,
  CM_MAX_CHANNEL_ID
};

enum cm_apll_error
{
  CM_NO_APPL_TRANS_SUPPORT=1,
  CM_APPL_TRANS_SUPPORT,
  CM_APPL_NULL_CALLBACK,
  CM_APPL_NO_RECORD,
  CM_APPL_START_TRANS_FAILED,
  CM_APPL_ADD_FAILED,
  CM_APPL_SET_FAILED,
  CM_APPL_DEL_FAILED,
  CM_APPL_TEST_FAILED,
  CM_APPL_GETFIRST_NREC_FAILED,
  CM_APPL_GETNEXT_NREC_FAILED,
  CM_APPL_GETEXACT_REC_FAILED,
  CM_APPL_END_TRANS_FAILED,
  CM_APPL_INVALID_CMD,

};

enum cm_cpdp_cmd_id
{
   CM_CPDP_APPL_TRANS_BEGIN_CMD=0,
   CM_CPDP_APPL_ADD_CMD,
   CM_CPDP_APPL_SET_CMD,
   CM_CPDP_APPL_DEL_CMD,
   CM_CPDP_APPL_TRANS_END_CMD,
   CM_CPDP_APPL_GET_FIRSTN_CMD,
   CM_CPDP_APPL_GET_NEXTN_CMD,
   CM_CPDP_APPL_GET_EACT_CMD,
   CM_CPDP_APPL_TEST_CMD,
   CM_CPDP_APPL_NOT_REG
};

struct cm_array_of_static_strings
{
  uint32_t count_ui;
  char **string_arr;
};

struct cm_array_of_strings
{
  uint32_t count_ui;
  char **string_arr_p;
};

struct cm_array_of_uints
{
  uint32_t count_ui;
  int32_t *uint_arr_p;
};

struct cm_array_of_structs
{
  uint32_t count_ui;
  void *struct_arr_p;
};

struct cm_int_enum
{
  uint32_t count_ui;
  int32_t array[5];
};

struct cm_uint_enum
{
  uint32_t count_ui;
  uint32_t array[5];
};

struct cm_strings_enum
{
  uint32_t count_ui;
  unsigned char array[5][50];
};

/* Configuration related callback functions */
typedef void *(*cm_start_xtn_cbk_p) (struct cm_array_of_iv_pairs
                                                            * arr_p,
                                                            uint32_t command_id,
                 struct cm_app_result ** result_p);
typedef int32_t (*cm_add_cbk_p) (void * config_trans_p,
                                               struct cm_array_of_iv_pairs * keys_arr_p,
                                               struct cm_array_of_iv_pairs * member_arr_p,
                                               struct cm_app_result ** result_p);

typedef int32_t (*cm_modify_cbk_p) (void * config_trans_p,
                                               struct cm_array_of_iv_pairs * keys_arr_p,
                                               struct cm_array_of_iv_pairs * member_arr_p,
                                               struct cm_app_result ** result_p);

typedef int32_t (*cm_delete_cbk_p) (void * config_transaction_p,
                                               struct cm_array_of_iv_pairs * keys_arr_p,
                                               struct cm_app_result ** result_p);

typedef int32_t (*cm_end_xtn_cbk_p) (void *
                                                          config_trans_p,
                                                          uint32_t command_id,
              struct cm_app_result ** result_p);

/* Information retrieval Callback functions */
typedef int32_t (*cm_getfirst_nrecs_cbk_p) (struct cm_array_of_iv_pairs *
                                                      keys_arr_p,
                                                      uint32_t * count_p,
                                                      struct cm_array_of_iv_pairs **
                                                      first_n_record_data_p);
typedef int32_t (*cm_getnext_nrecs_cbk_p) (struct cm_array_of_iv_pairs *
                                                     keys_arr_p,
                                                     struct cm_array_of_iv_pairs *
                                                     prev_record_key_p,
                                                     uint32_t * count_p,
                                                     struct cm_array_of_iv_pairs **
                                                     next_n_record_data_p);
typedef int32_t (*cm_getexact_rec_cbk_p) (struct cm_array_of_iv_pairs *
                                                    keys_arr_p,
                                                    struct cm_array_of_iv_pairs **
                                                    record_data_p);

typedef int32_t (*cm_test_cbk_p) (struct cm_array_of_iv_pairs * keys_arr_p,
                                                uint32_t command_id,
                                                struct cm_app_result ** result_p);

typedef int32_t (*cm_isitok_cbk_p) (void *
                                                          config_trans_p,
                                                          uint32_t command_id,
                                                    struct cm_app_result ** result_p);
struct cm_dm_app_callbacks
{
  cm_start_xtn_cbk_p cm_start_xtn;
  cm_add_cbk_p cm_add_rec;
  cm_modify_cbk_p cm_modify_rec;
  cm_delete_cbk_p cm_delete_rec;
  cm_end_xtn_cbk_p cm_end_xtn;
  cm_getfirst_nrecs_cbk_p cm_getfirst_nrecs;
  cm_getnext_nrecs_cbk_p cm_getnext_nrecs;
  cm_getexact_rec_cbk_p cm_getexact_rec;
  cm_test_cbk_p cm_test_config;
  cm_isitok_cbk_p cm_isitok_to_commit;
};

int32_t UCMDPRegisterApplicationCallbacks(uint32_t application_id,struct cm_dm_app_callbacks *call_backs_p);
/* Configuration channel related callback functions */
int32_t cm_register_app_callbacks(uint32_t application_id,struct cm_dm_app_callbacks *call_backs_p);
typedef void *(*cm_channel_start_xtn_cbk_p) (uint32_t appl_id,struct cm_array_of_iv_pairs * arr_p,
                                                            uint32_t command_id,struct cm_app_result ** result_p);
typedef int32_t (*cm_channel_add_cbk_p) (uint32_t appl_id,void * config_trans_p,
                                               struct cm_array_of_iv_pairs * keys_arr_p,
                                               struct cm_array_of_iv_pairs * member_arr_p,
                                               struct cm_app_result ** result_p);

typedef int32_t (*cm_channel_modify_cbk_p) (uint32_t appl_id,void * config_trans_p,
                                               struct cm_array_of_iv_pairs * keys_arr_p,
                                               struct cm_array_of_iv_pairs * member_arr_p,
                                               struct cm_app_result ** result_p);

typedef int32_t (*cm_channel_delete_cbk_p) (uint32_t appl_id,void * config_transaction_p,
                                               struct cm_array_of_iv_pairs * keys_arr_p,
                                               struct cm_app_result ** result_p);

typedef int32_t (*cm_channel_end_xtn_cbk_p) (uint32_t appl_id,void *
                                                          config_trans_p,
                                                          uint32_t command_id,struct cm_app_result ** result_p);

/* Information retrieval Callback functions */
typedef int32_t (*cm_channel_getfirst_nrecs_cbk_p) (uint32_t appl_id,struct cm_array_of_iv_pairs *
                                                      keys_arr_p,
                                                      uint32_t * count_p,
                                                      struct cm_array_of_iv_pairs **
                                                      first_n_record_data_p);
typedef int32_t (*cm_channel_getnext_nrecs_cbk_p) (uint32_t appl_id,struct cm_array_of_iv_pairs *
                                                     keys_arr_p,
                                                     struct cm_array_of_iv_pairs *
                                                     prev_record_key_p,
                                                     uint32_t * count_p,
                                                     struct cm_array_of_iv_pairs **
                                                     next_n_record_data_p);
typedef int32_t (*cm_channel_getexact_rec_cbk_p) (uint32_t appl_id,struct cm_array_of_iv_pairs *
                                                    keys_arr_p,
                                                    struct cm_array_of_iv_pairs **
                                                    record_data_p);

typedef int32_t (*cm_channel_test_cbk_p) (uint32_t appl_id,struct cm_array_of_iv_pairs * keys_arr_p,
                                                uint32_t command_id,
                                                struct cm_app_result ** result_p);

struct cm_dm_channel_callbacks
{
  cm_channel_start_xtn_cbk_p cm_start_xtn;
  cm_channel_add_cbk_p cm_add_rec;
  cm_channel_modify_cbk_p cm_modify_rec;
  cm_channel_delete_cbk_p cm_delete_rec;
  cm_channel_end_xtn_cbk_p cm_end_xtn;
  cm_channel_getfirst_nrecs_cbk_p cm_getfirst_nrecs;
  cm_channel_getnext_nrecs_cbk_p cm_getnext_nrecs;
  cm_channel_getexact_rec_cbk_p cm_getexact_rec;
  cm_channel_test_cbk_p cm_test_config;
};

struct cm_channel_applcallback
{
  uint32_t appl_id_ui;
  struct cm_dm_app_callbacks app_call_backs;
  struct cm_channel_applcallback *next_p;
};
/* Element Types */
#define CM_DMNODE_TYPE_ANCHOR        0x0
#define CM_DMNODE_TYPE_TABLE         0x1
#define CM_DMNODE_TYPE_SCALAR_VAR    0x2
#define CM_DMNODE_TYPE_INVALID       0x3

/* Element Permissions */
#define CM_VORTIQA_SUPERUSER_ROLE "VortiQaRole" /* Top Level Permissions*/
#define CM_DM_DEFAULT_ROLE "UCMRole"   /*Supervisor Permissions*/
#define CM_DM_ROLE1 "AdminRole"        /*Configurable Permissions*/ 
#define CM_DM_ROLE2 "GuestRole"        /*Read Only Permissions*/
#define CM_DM_ROLE3 "UserRole"         /*Read Only Permissions*/

#define CM_PERMISSION_NOACCESS       0x0
#define CM_PERMISSION_READ_ONLY      0x1
#define CM_PERMISSION_READ_WRITE     0x2
#define CM_PERMISSION_READ_ROLEPERM    0x3
#define CM_PERMISSION_READ_WRITE_ROLEPERM   0x4

#define CM_ACTIVE_NOTIFICATION   0x01
#define CM_PASSIVE_NOTIFICATION  0x02
#define CM_NOT_CHANGEABLE_BY_ACS 0x03

/* UCM Data Types */
#define CM_DATA_TYPE_UNKNOWN         0x0
#define CM_DATA_TYPE_STRING          0x1
#define CM_DATA_TYPE_INT             0x2
#define CM_DATA_TYPE_UINT            0x3
#define CM_DATA_TYPE_BOOLEAN         0x4
#define CM_DATA_TYPE_DATETIME        0x5
#define CM_DATA_TYPE_BASE64          0x6
#define CM_DATA_TYPE_IPADDR          0x7
#define CM_DATA_TYPE_IPV6ADDR        0x8
#define CM_DATA_TYPE_EXCLUDING_XMLSTRINGS          0x9
#define CM_DATA_TYPE_STRING_SPECIAL_CHARS          0xa
#define CM_DATA_TYPE_STRING_PASSWORD          0xb
#define CM_DATA_TYPE_INT64             0xc
#define CM_DATA_TYPE_UINT64             0xd

/* Element value attrib type */
#define CM_DATA_ATTRIB_NONE          0x0
#define CM_DATA_ATTRIB_INT_RANGE     0x1
#define CM_DATA_ATTRIB_UINT_RANGE    0x2
#define CM_DATA_ATTRIB_STR_RANGE     0x3
#define CM_DATA_ATTRIB_INT_ENUM      0x4
#define CM_DATA_ATTRIB_UINT_ENUM     0x5
#define CM_DATA_ATTRIB_STR_ENUM      0x6
#define CM_DATA_ATTRIB_IPADDR_ENUM   0x7
#define CM_DATA_ATTRIB_IPADDR_RANGE  0x8
#define CM_DATA_ATTRIB_IPV6ADDR_ENUM 0x9
#define CM_DATA_ATTRIB_IPV6ADDR_RANGE 0xa
#define CM_DATA_ATTRIB_STR_MULTVAL 0xb
#define CM_DATA_ATTRIB_STR_MULTVAL_NOVALD 0xc
#define CM_DATA_ATTRIB_INT64_RANGE     0xd
#define CM_DATA_ATTRIB_UINT64_RANGE    0xe
#define CM_DATA_ATTRIB_INT64_ENUM      0xf
#define CM_DATA_ATTRIB_UINT64_ENUM     0x10
#define CM_DATA_ATTRIB_STR_LARGE_ENUM 0x0

#define CM_ACCESS_ALL 0x00
#define CM_ACCESS_NO_HTTP 0x01
#define CM_ACCESS_NO_CLI 0x02


struct cm_dm_element_attrib
{
  uint8_t element_type:3;      /* anchor/table/scalar variable */
  unsigned char visible_b:1;          /* should we expose for configuration? */
  unsigned char key_b:1;              /* Is this a key for the container? */
  unsigned char mandatory_b:1;        /* Is this a mandatory param? */
  unsigned char ordered_table_b:1;     /* Is this an ordered list? (table) */
  unsigned char table_priority_b:1;    /* Is this specifies priority of 
                                   container table? */
  unsigned char notify_change_b:1;     /* Inform if any config change on this 
                                   element */
  unsigned char notify_attrib:3;      /* 1 = ActiveNotify,
                                   2 = PassiveNotify
                                   4 = Not changeable by ACS */

  unsigned char reboot_on_change_b:1;   /* Reboot required if modified */
  unsigned char sticky_to_parent_b:1;   /* This node config should go along 
                                   with parent config */
  unsigned char access_controle_b:2;    /* 0 = Allowed from ALL
                                   1 = HTTP Can not Edit
                                   2 = CLI Can not Edit */
  unsigned char scalar_group_b:1;      /* 0 = if individual scalar
                                   1 = if registered as a group of scalars */
  unsigned char global_trans_b:1;      /* Is this node supports global transaction */
  unsigned char parent_trans_b:1;   /* Is this node configuartion should go as part of parent transaction. */
  unsigned char system_command_b:1;         /* Is this a system command */
  unsigned char stats_command_b:1;       /* Is this a stats command */
	unsigned char device_specific_b:1; /* Is this Device Specific command */
  int16_t padbits:10;          /* Unused bits */


  unsigned char save_in_parent_instance_dir_b;        /* Save this element in parent instance's
                                           directory */
  unsigned char non_config_leaf_node_b;  /* This is a non-configurable leaf node.
                                   Like structure members. */
  int16_t pad;            /* Unused bytes            */

  char *command_name_p;       /* alias name for table can be used as a command name*/
  char *cli_identifier_p;     /* identifier for optional prameter */
  char *cli_idenetifier_help_p; /* help for identifier */
  
};

struct cm_dm_data_attrib
{
  unsigned char data_type;        /* int/unsigned int/string/boolean/base64/ip/ipv6 */
  unsigned char attrib_type;      /* range/enum/etc */
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
      int32_t array[CM_MAX_INT_ENUM_COUNT];
    } int_enum;
    struct
    {
      uint32_t count_ui;
      uint32_t array[CM_MAX_INT_ENUM_COUNT];
    } uint_enum;
    struct
    {
      uint32_t count_ui;
      int64_t array[CM_MAX_INT_ENUM_COUNT];
    } longint_enum;
    struct
    {
      uint32_t count_ui;
      uint64_t array[CM_MAX_INT_ENUM_COUNT];
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
      char start_ip[128];
      char end_ip[128];
    } ipv6_range;
    struct
    {
      uint32_t count_ui;
      unsigned char ip_v6_arr_a[5][128];
    } ipv6_enum;
    uint32_t max_no_of_tables;
  } attr;
  union
  {
    uint64_t uldefault_value;
    int64_t  ldefault_value;
    uint32_t uidefault_value;
    int32_t idefault_value;
    char default_string[50];
  } default_value;
};

#define CM_SAVE_FILE_NAME_MAX_LEN  50
/* Data Element Info structure */
struct cm_dm_node_info
{
  char *name_p;
  char *friendly_name_p;
  char *description_p;
  char *dm_path_p;
  uint32_t id_ui;
  char file_name[CM_SAVE_FILE_NAME_MAX_LEN];        /*LDSV filename */
  unsigned char save_b;

  struct cm_dm_element_attrib element_attrib;
  struct cm_dm_data_attrib data_attrib;
};

#define UCMDM_ADMIN_ROLE_MAX_LEN  32
struct cm_dm_role_permission
{
  char role[UCMDM_ADMIN_ROLE_MAX_LEN + 1];
  uint32_t permissions;
};

struct cm_dm_array_of_role_permissions
{
   uint32_t count_ui;
   struct cm_dm_role_permission *role_permissions;
};
/*---------------------------------------------------------------------
   Instance_ID : Key map related structures 
 ---------------------------------------------------------------------*/
struct cm_dm_instance_map
{
  uint32_t instance_id;
  uint32_t key_type;            /* Use UCM Data Types */
  uint32_t key_length;
  void *key_p;
  struct cm_array_of_nv_pair key_nv_pairs;
};

typedef int32_t (*UCMDM_VALIDATE_FNPTR) (void * arg1, void * arg2,
                                         void * arg3, void * arg4);

/*--------------------------------------------------------------------------
  Every element that can be accessed via management interfaces
  should be defined using this data structure. 
 *-------------------------------------------------------------------------*/
struct cm_dm_data_element
{
  /* Reserved section */
  struct cm_dm_data_element *parent_p;  /* Reserved : internal use */
  struct cm_dm_app_callbacks *app_cbks_p;  /* Reserved : internal use */
  UCMDllQ_t child_list;          /* Reserved : internal use */
  UCMDllQNode_t list_node;       /* Reserved : internal use */
  UCMDllQ_t role_perm_list;       /* Reserved : internal use */

  /* User section */
  char *name_p;               /* Element name. Not an FQN */
  char *friendly_name_p;               /* Element name. Not an FQN */
  uint32_t id_ui;                /* Unique ID with in siblings */
  struct cm_array_of_static_strings *ref_names_p; /* Reference name array */
  UCMDM_VALIDATE_FNPTR validate_func_p;   /* Function ptr to validate data */
  char *description_p;        /* Short description */
  char *save_dir_name_p;         /* Directort Name to save configuration */
  char *save_file_name_p;       /* If NULL, use parent's filename */

  struct cm_dm_element_attrib element_attrib;
  struct cm_dm_data_attrib data_attrib;

};

/* Instance Path element : useful for specifying the
   Instance path while using Instance related API.
 */
struct cm_dm_id_path_element
{
  /* Node or Object Identifier */
  uint32_t element_id;
  uint8_t instance_key_type;
  union
  {
    uint32_t int_key_ui;
    unsigned char *string_key_p;
  } instance_key;
};

struct cm_dm_id_path
{
  /* Number of Path Elements */
  uint32_t no_of_path_elements;

  /* Array of path elements */
  struct cm_dm_id_path_element *id_path_arr_p;
};

#endif /* UCMDM_GDEF_H */
