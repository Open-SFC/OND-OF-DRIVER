

#define NSRM_SUCCESS 0
#define NSRM_FAILURE 1

 
/*Common macros*/
#define NSRM_ERROR_DUPLICATE_RESOURCE                           -1
#define NSRM_ERROR_NO_MORE_ENTRIES                              -2


/*Database specific*/
#define NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID                -3
#define NSRM_NWSERVICE_OBJECT_ADD_FAIL                          -4
#define NSRM_NWSERVICE_OBJECT_DEL_FAIL                          -5
#define NSRM_NWSERVICE_OBJECT_MOD_FAIL                          -6
#define NSRM_NWSERVICE_OBJECT_ATTACH_TO_NSCHAIN_FAIL            -7
#define NSRM_NWSERVICE_OBJECT_DETACH_FROM_NSCHAIN_FAIL          -8


#define NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID                  -9
#define NSRM_NSCHAIN_OBJECT_ADD_FAIL                            -10
#define NSRM_NSCHAIN_OBJECT_DEL_FAIL                            -11
#define NSRM_NSCHAIN_OBJECT_MOD_FAIL                            -12 


#define NSRM_ERROR_NSCHAINSET_OBJECT_NAME_INVALID               -13
#define NSRM_NSCHAINSET_OBJECT_ADD_FAIL                         -14
#define NSRM_NSCHAINSET_OBJECT_DEL_FAIL                         -15
#define NSRM_NSCHAINSET_OBJECT_MOD_FAIL                         -16 


#define NSRM_ERROR_L2NW_SERVICE_MAP_NAME_INVALID                -17
#define NSRM_L2NW_SERVICE_MAP_ADD_FAIL                          -18
#define NSRM_L2NW_SERVICE_MAP_DEL_FAIL                          -19
#define NSRM_L2NW_SERVICE_MAP_MOD_FAIL                          -20 

#define NSRM_ERROR_NWSERVICE_INSTANCE_NAME_NULL                 -21
#define NSRM_NWSERVICE_INSTANCE_LAUNCH_FAIL                     -22
#define NSRM_NWSERVICE_INSTANCE_DELAUNCH_FAIL                   -23


#define NSRM_ERROR_INVALID_SELECTION_RULE_NAME                  -25 
#define NSRM_SELECTION_RULE_ADD_FAIL                            -26 
#define NSRM_SELECTION_RULE_DEL_FAIL                            -27 

#define NSRM_ERROR_INVALID_BYPASS_RULE_NAME                     -28 
#define NSRM_BYPASS_RULE_ADD_FAIL                               -29 
#define NSRM_BYPASS_RULE_DEL_FAIL                               -30 

#define NSRM_ERROR_LENGTH_NOT_ENOUGH                            -31
#define NSRM_NSCHAIN_USED_IN_SELECTION_RULE                     -32
#define NSRM_NWSERVICE_USED_IN_BYPASS_RULE                      -33
#define NSRM_ERROR_ATTRIBUTE_NAME_NULL                          -34
#define NSRM_ERROR_ATTRIBUTE_VALUE_NULL                         -35
#define NSRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN                   -36
#define NSRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN                  -37
#define NSRM_ERROR_NO_MEM                                       -38
#define NSRM_ERROR_INVALID_NAME                                 -39
/*Max name length . TO be used by the caller*/
#define  NSRM_MAX_NAME_LENGTH                                    128
/*
Databases:
---------
 1.  nwservice_object
 2.  nschain_object
 3.  nwservice_bypass_rule 
 4.  nschainset_object
 5.  nschain_selection_rule
 6.  l2nw_service_map
 7.  l3nw_service_map

API Types for each database:
---------------------------
 1.  basic API
     A.  add_object
     B.  del_object
     C.  modify_object
     D.  get_first_objects
     E.  get_next_objects
     F.  get_exact_object
     G.  attach object to another object
     H.  detach object from another object
 2.  attributes API
     A.  add_attribute_to_object
     B.  del_attribute_from_object
     C.  get_first_attribute
     D.  get_next_attribute
     E.  get_exact_attribute
 3.  notifications API
     A.  Notification Registration API 
     B.  Callback Notifier
     C.  Object Notification Data structure 
     D.  Notification Types
 4.  external views API
     A.  create_view
     B.  get_first_view_entries
     C.  get_next_view_entries

Add_attribute and Del_attribute API internally create/delete view entries */

/*---------------------------------------------------------------------*/
/* Common Structures and API across all NSRM databases */

/*Structures to add attributes*/

struct nsrm_attribute_name_value_pair
{
  char* attribute_name_p;
  uint32_t name_length;
  char* attribute_value_p;
  uint32_t value_length;
};

struct nsrm_attribute_name_value_output_info
{
  char* name_p;
  uint32_t attr_length;
  char* value_p;
  uint32_t val_length;
};

enum nwservice_type_e  /* chain_type_e NSM1 */ 
{
  L2 = 0,
  L3
};

enum admin_status_e
{
  ADMIN_ENABLE,
  ADMIN_DISABLE
};

typedef enum 
{
  OPER_ENABLE=1,
  OPER_DISABLE
}oper_status_e;

struct nsrm_ip
{
  uint8_t  ip_object_type_e; /* ANY, single, range, subnet */
  uint32_t ipaddr_start;
  uint32_t ipaddr_end; /* numberof mask bits in case of subnet */
};

enum ip_object_type_e
{
  IP_ANY = 0,
  IP_SINGLE,
  IP_RANGE,
  IP_SUBNET
};

/* This structure is exposed to Configuration Application like Open Stack */
struct nsrm_port
{
  uint8_t  port_object_type_e; /* ANY ,single, range */
  uint16_t port_start;
  uint16_t port_end;
};

enum port_object_type_e
{
  PORT_ANY = 0,
  PORT_SINGLE,
  PORT_RANGE
};

struct nsrm_mac
{
  uint8_t mac_address_type_e; /* ANY, single */
  uint8_t mac[6];
};

enum mac_address_type_e
{
  MAC_ANY = 0,
  MAC_SINGLE
};

struct nsrm_ethtype
{
  uint8_t  ethtype_type_e; /* ANY, single */
  uint16_t ethtype;
};

enum ethtype_type_e
{
  ETHTYPE_ANY = 0,
  ETHTYPE_SINGLE
};

/*enums required for nschain_object*/

enum location_e
{
  FIRST = 0,
  LAST,
  BEFORE,
  AFTER
};

enum load_indication_type_e
{
  CONNECTION_BASED = 0,
  TRAFFIC_BASED
};

enum load_sharing_algorithm_e
{
  HASH_BASED_ALGORITHM = 0,
  ROUND_ROBIN,
  LEAST_CONNECTIONS
};

/*---------------------------------------------------------------------*/
/*Structure defined to attach nwservice object to nschain object*/
struct nsrm_nwservice_attach_key
{
  char*   name_p;
  char*   nschain_object_name_p;
  uint16_t sequence_number;
};


/* 1. nsrm_nwservice_object */

struct nsrm_nwservice_object_key 
{
  char* name_p;
  char* tenant_name_p;
};

struct nsrm_nwservice_object_config_info
{
  uint32_t field_id;
  union
  {
    uint8_t  nwservice_object_form_factor_type_e;
    uint8_t  admin_status_e;
    uint8_t  nwservice_type_e;  
    uint8_t  load_sharing_algorithm_e;
    uint8_t  load_indication_type_e;
    uint64_t high_threshold;
    uint64_t low_threshold;
  }value;
};

/* This structure is used in get_first,get_next,get_exact API */
struct nsrm_nwservice_object_record
{
  struct  nsrm_nwservice_object_key           key;
  struct  nsrm_nwservice_object_config_info*  info;
}; 

#define NSRM_MAX_NWSERVICE_OBJECT_CONFIG_PARAMETERS 7  /* 2 NSM */

/* Field IDs defined for this database: 
   0000 - nwservice_object_form_factor_type_e  -  Physical, Virtual
   0001 - admin_status_e - Enable / Disable
   0002 - nwservice_type_e - L2,L3    
   0003 - load_sharing_algorithm_e;
   0004 - load_indication_type_e;
   0005 - high_threshold;
   0006 - low_threshold;
*/

/* API */
int32_t nsrm_add_nwservice_object(
        int32_t number_of_config_params,
        struct  nsrm_nwservice_object_key*         key_info_p,
        struct  nsrm_nwservice_object_config_info* config_info_p
      );


int32_t nsrm_del_nwservice_object(
          struct nsrm_nwservice_object_key*   key_info_p
        );

int32_t nsrm_modify_nwservice_object(
          struct  nsrm_nwservice_object_key*           key_info_p,
          int32_t number_of_config_params,
          struct  nsrm_nwservice_object_config_info*   config_info_p
          );


int32_t nsrm_get_first_nwservice_objects(
         int32_t  number_of_nwservice_objects_requested, 
         int32_t* number_of_nwservice_objects_returned_p, 
         struct   nsrm_nwservice_object_record* recs_p);

int32_t nsrm_get_next_nwservice_object(
        struct   nsrm_nwservice_object_key* relative_record_key_p,
        int32_t  number_of_nwservice_objects_requested,
        int32_t* number_of_nwservice_objects_returned_p,
        struct   nsrm_nwservice_object_record* recs_p);

int32_t nsrm_get_exact_nwservice_object(
        struct   nsrm_nwservice_object_key*   key_info_p,
        struct   nsrm_nwservice_object_record*   rec_p
     );

/* Attributes */
int32_t nsrm_add_attribute_to_nwservice_object(
          char*     nwservice_object_name_p, 
          struct    nsrm_attribute_name_value_pair *attribute_info_p);

int32_t nsrm_del_attribute_from_nwservice_object(
          char* nwservice_object_name_p, 
          char* attribute_name_p);

int32_t nsrm_modify_attribute_in_nwservice_object(
          char*   nwservice_object_name_p, 
          struct  nsrm_attribute_name_value_pair *attribute_info_p);


int32_t nsrm_get_first_nwservice_object_attribute(
          char*   nwservice_object_name_p, 
          struct  nsrm_attribute_name_value_output_info *attribute_output_p
        );
      
int32_t nsrm_get_next_nwservice_object_attribute(
          char*  nwservice_object_name_p, 
          char*  current_attribute_name_p,
          struct nsrm_attribute_name_value_output_info *attribute_output_p
        );

int32_t nsrm_get_exact_nwservice_object_attribute(
          char*  nwservice_object_name_p,
          char*  attribute_name_p,
          struct nsrm_attribute_name_value_output_info *attribute_output_p
        );

/* Views */
int32_t nsrm_create_nwservice_object_view(char* view_p);

int32_t nsrm_get_first_nwservice_object_view_entries(
            char*     view_name_p,
            int32_t   number_of_nwservice_objects_requested,
            int32_t*  number_of_nwservice_objects_returned_p,
            struct    nsrm_nwservice_object_record* recs_p
          );

int32_t nsrm_get_next_nwservice_object_view_entries(
            char*    view_name_p,
            struct   nsrm_nwservice_object_key* relative_record_key_p,
            int32_t  number_of_nwservice_objects_requested,
            int32_t* number_of_nwservice_objects_returned_p,
            struct   nsrm_nwservice_object_record* recs_p);

/* Notifications */

struct nsrm_nwservice_object_notifications
{
  char*     nwservice_object_name_p;
  uint64_t  nwservice_object_handle;
};

struct nsrm_nwservice_object_attachment_notifications
{
  char*     nwservice_object_name_p;
  uint64_t  nwservice_object_handle;

  char*     nschain_object_name_p;
  uint64_t  nschain_object_handle;
};

struct nsrm_nwservice_object_launch_notifications
{
  char*     nwservice_object_name_p;
  uint64_t  nwservice_object_handle;

  char*     nschain_object_name_p;
  uint64_t  nschain_object_handle;

  char*     vm_name_p;
  uint64_t  vm_handle;
  uint64_t  port_handle;
  uint16_t  vlan_id_in;
  uint16_t  vlan_id_out;

  uint64_t  port_saferef; /*NSM added*/
};

struct nwservice_object_attribute_notifications
{
  char*     nwservice_object_name_p;
  uint64_t  nwservice_object_handle;
  char*     nwservice_attribute_name_p;
  char*     nwservice_attribute_value_p;
};

/*notification data structure*/
union nsrm_nwservice_object_notification_data
{
  struct nsrm_nwservice_object_notifications add_del;
  struct nsrm_nwservice_object_attachment_notifications attach;
  struct nsrm_nwservice_object_launch_notifications launch;
  struct nwservice_object_attribute_notifications attribute;
};

/*Callback for notifications*/
typedef void(*nsrm_nwservice_object_notifier)
         (uint8_t notification_type,
          uint64_t nwservice_handle,
          union nsrm_nwservice_object_notification_data notification_data,
          void *callback_arg1,
          void *callback_arg2);

/* Registration API */
int32_t nsrm_register_nwservice_object_notifications( 
            uint8_t notification_type,
            nsrm_nwservice_object_notifier  nwservice_notifier,
            void* callback_arg1,
            void* callback_arg2); 

enum nwservice_object_notification_type
{
    NSRM_NWSERVICE_OBJECT_ALL = 0,
    NSRM_NWSERVICE_OBJECT_ADDED,
    NSRM_NWSERVICE_OBJECT_DELETED,
    NSRM_NWSERVICE_OBJECT_ATTACHED_TO_NSCHAIN_OBJECT,
    NSRM_NWSERVICE_OBJECT_DETACHED_FROM_NSCHAIN_OBJECT,
    NSRM_NWSERVICE_OBJECT_LAUNCHED,
    NSRM_NWSERVICE_OBJECT_DE_LAUNCHED,
    NSRM_NWSERVICE_OBJECT_ATTRIBUTE_ADDED,
    NSRM_NWSERVICE_OBJECT_ATTRIBUTE_DELETED
};

enum nwservice_object_form_factor_type
{
  PHYSICAL = 2,
  VIRTUAL
};

struct nsrm_nwservice_instance_key
{
   char*    nwservice_instance_name_p;
   char*    tenant_name_p;
   char*    nschain_object_name_p;
   char*    nwservice_object_name_p;
   char*    vm_name_p;
   uint16_t vlan_id_in;
   uint16_t vlan_id_out;

};

struct nsrm_nwservice_instance_record
{
  struct nsrm_nwservice_instance_key  key;
};

/* Launching and delaunching nwservice_object Instances */
int32_t nsrm_register_launched_nwservice_object_instance(
         struct nsrm_nwservice_instance_key* key);

int32_t nsrm_register_delaunch_nwswervice_object_instance(
         struct nsrm_nwservice_instance_key* key 
        );

int32_t nsrm_get_first_nwservice_instance(
                         struct   nsrm_nwservice_attach_key    *nschain_nwservice_object_p,
                         int32_t  number_of_nwservice_instances_requested,
                         int32_t* number_of_nwservice_instances_returned_p,
                         struct   nsrm_nwservice_instance_record* recs_p);

int32_t nsrm_get_next_nwservice_instance(
                         struct   nsrm_nwservice_attach_key    *nschain_nwservice_object_p,
                         struct   nsrm_nwservice_instance_key   *relative_record_key_p,
                         int32_t  number_of_nwservice_instances_requested,
                         int32_t* number_of_nwservice_instances_returned_p,
                         struct   nsrm_nwservice_instance_record* recs_p);

int32_t nsrm_get_exact_nwservice_instance(
        struct   nsrm_nwservice_attach_key         *nschain_nwservice_object_p,
        struct   nsrm_nwservice_instance_key*      key_info_p,
        struct   nsrm_nwservice_instance_record*   rec_p);


/*--------------------------------------------------------------*/                                         

/* 2. nschain objects */
struct nsrm_nschain_object_key
{
  char*  name_p;
  char*  tenant_name_p;
};

struct nsrm_nschain_object_config_info
{
  uint32_t field_id;
  union
  {
/* NSM
    uint8_t  nschain_type; 
    uint8_t  load_sharing_algorithm_e;
    uint8_t  packet_fields_to_hash; 
    uint8_t  load_indication_type_e;
    uint64_t high_threshold;
    uint64_t low_threshold;
*/    
    uint8_t  packet_fields_to_hash; 
    uint8_t  admin_status_e;
  }value;
};

/*New super structure */
struct nsrm_nschain_object_record
{
  struct nsrm_nschain_object_key          key;
  struct nsrm_nschain_object_config_info*  info;
};

#define NSRM_MAX_NSCHAIN_OBJECT_CONFIG_PARAMETERS  2  /* 6 NSM */

/* Field ID's defined for this database:  
   0000 -  packet_fields_to_hash;
   0001 -  admin_status; */ 
#if 0
   0001 -  load_sharing_algorithm_e;
   0002 -  packet_fields_to_hash; 
   0003 -  load_indication_type_e;
   0004 -  high_threshold;
   0005 -  low_threshold;
#endif

int32_t nsrm_add_nschain_object(
        int32_t number_of_config_params,
        struct  nsrm_nschain_object_key*           key_info_p,
        struct  nsrm_nschain_object_config_info*   config_info_p
      );

int32_t nsrm_del_nschain_object(
        struct  nsrm_nschain_object_key*           key_info_p
        );

/* Allowed only if all the bypass rules in the nschain_object are deleted and all the nwservice_objects are detached 
   from the nschain_object and the nschain_usage_count variable in nschain_object is 0 which indicates that no
   nschainset_object is referring to this nschain_object. */

int32_t nsrm_modify_nschain_object(
        struct  nsrm_nschain_object_key*           key_info_p,
        int32_t number_of_config_params,
        struct  nsrm_nschain_object_config_info*   config_info_p
      );


int32_t nsrm_get_first_nschain_objects(
        int32_t  number_of_nschain_objects_requested, 
        int32_t* number_of_nschain_objects_returned_p, 
        struct   nsrm_nschain_object_record* recs_p
        );


int32_t nsrm_get_next_nschain_objects(
        struct   nsrm_nschain_object_key*   relative_record_key_p,
        int32_t  number_of_nschain_objects_requested,
        int32_t* number_of_nschain_objects_returned_p,
        struct   nsrm_nschain_object_record* recs_p);

int32_t nsrm_get_exact_nschain_object(
        struct   nsrm_nschain_object_key*   key_info_p,
        struct   nsrm_nschain_object_record*   rec_p
     );

int32_t nsrm_attach_nwservice_object_to_nschain_object(
          struct  nsrm_nwservice_attach_key* key_info_p);

int32_t nsrm_detach_nwservice_object_from_nschain_object(
          struct  nsrm_nwservice_attach_key* key_info_p);

int32_t nsrm_get_first_nwservice_objects_in_nschain_object(
             struct   nsrm_nschain_object_key* nschain_object_key_p,
             int32_t  number_of_nwservice_objects_requested,
             int32_t* number_of_nwservice_objects_returned_p,
             struct   nsrm_nwservice_attach_key* recs_p);

int32_t nsrm_get_next_nwservice_object_in_nschain_object(
            struct   nsrm_nschain_object_key* nschain_object_key_p,
            struct   nsrm_nwservice_object_key* relative_record_key_p,
            int32_t  number_of_nwservice_objects_requested,
            int32_t* number_of_nwservice_objects_returned_p,
            struct   nsrm_nwservice_attach_key* recs_p);

int32_t nsrm_get_exact_nwservice_object_in_nschain_object(
            struct   nsrm_nschain_object_key* nschain_object_key_p,
            struct   nsrm_nwservice_object_key*   key_info_p,
            struct   nsrm_nwservice_attach_key*   rec_p);




/* Attributes */

int32_t nsrm_add_attribute_to_nschain_object(
          char*     nschain_object_name_p, 
          struct    nsrm_attribute_name_value_pair *attribute_info_p);

int32_t nsrm_del_attribute_from_nschain_object(
           char*    nschain_object_name_p, 
           char* attribute_name_p);

int32_t nsrm_modify_attribute_in_nschain_object(
          char*     nschain_object_name_p, 
          struct    nsrm_attribute_name_value_pair *attribute_info_p);

int32_t nsrm_get_first_nschain_object_attribute(
          char*  nschain_object_name_p, 
          struct nsrm_attribute_name_value_output_info *attribute_output_p);
      

int32_t nsrm_get_next_nschain_object_attribute(
          char*  nschain_object_name_p, 
          char*  current_attribute_name_p,
          struct nsrm_attribute_name_value_output_info *attribute_output_p);


int32_t nsrm_get_exact_nschain_object_attribute(
            char*   nschain_object_name_p,
            char*   attribute_name_p,
            struct  nsrm_attribute_name_value_output_info *attribute_output_p
          );

/* Views */

int32_t nsrm_create_nschain_object_view(char* view_p);

int32_t nsrm_get_first_nschain_object_view_entries(
            char*     view_name_p,
            int32_t   number_of_nschain_objects_requested,
            int32_t*  number_of_nschain_objects_returned_p,
            struct    nsrm_nschain_object_record* recs_p
          );

int32_t nsrm_get_next_nschain_object_view_entries(
            char*    view_name_p,
            struct   nsrm_nschain_object_key* relative_record_key_p,
            int32_t  number_of_nschain_objects_requested,
            int32_t* number_of_nschain_objects_returned_p,
            struct   nsrm_nschain_object_record* recs_p
          );

/* Notifications */

struct  nsrm_nschain_object_notification
{
  char*     nschain_object_name_p;
  uint64_t  nschain_object_handle;
};
struct  nsrm_nschain_object_attribute_notification
{
  char*     nschain_object_name_p;
  uint64_t  nschain_object_handle;

  char*     nschain_attribute_name_p;
  char*     nschain_attribute_value_p;
};

union nsrm_nschain_object_notification_data
{
  struct  nsrm_nschain_object_notification add_del;
  struct  nsrm_nschain_object_attribute_notification attribute;
};

/*Callback for notifications*/
typedef void(*nsrm_nschain_object_notifier)
         (uint8_t notification_type,
         uint64_t nschain_handle,
         union  nsrm_nschain_object_notification_data notification_data,
         void *callback_arg1, void *callback_arg2);


/* Registration API */
int32_t nsrm_register_nschain_object_notifications( 
            uint8_t notification_type,
            nsrm_nschain_object_notifier  nschain_notifier,
            void* callback_arg1,
            void* callback_arg2); 

enum nwservice_nschain_object_notification_type
{
  NSRM_NSCHAIN_OBJECT_ALL = 0,
  NSRM_NSCHAIN_OBJECT_ADDED,
  NSRM_NSCHAIN_OBJECT_DELETED,
  NSRM_NSCHAIN_OBJECT_ATTRIBUTE_ADDED,
  NSRM_NSCHAIN_OBJECT_ATTRIBUTE_DELETED
};  

/* --------------------------------------------------------------------*/

/* 3. nwservice_bypass_rule */

struct nsrm_nwservice_bypass_rule_key
{
  char*  name_p;
  char*  nschain_object_name_p;
  char*  tenant_name_p;
  uint8_t location_e;
};
struct nsrm_nwservice_bypass_rule_config_info
{
  uint32_t field_id;
  union
  {
    struct nsrm_mac src_mac;
    struct nsrm_mac dst_mac;
    struct nsrm_ethtype eth_type;
    struct nsrm_ip   sip;
    struct nsrm_ip   dip;
    struct nsrm_port sp;
    struct nsrm_port dp;
    uint16_t ip_protocol;

    char*   relative_name_p;
    uint8_t admin_status_e;
  }value;
};

struct nsrm_nwservice_bypass_rule_record
{
  struct nsrm_nwservice_bypass_rule_key            key;
  struct nsrm_nwservice_bypass_rule_config_info*   info;
};

struct nsrm_bypass_rule_nwservice_key
{
  char* nschain_object_name_p;
  char* bypass_rule_name_p;
  char* nwservice_object_name_p;
};

#define NSRM_MAX_BYPASS_RULE_CONFIG_PARAMETERS  11

/* Field ID's defined for this database: 
   0001 -  src_mac 
   0002 -  dst_mac 
   0003 -  eth_type
   0004 -  sip
   0005 -  dip
   0006 -  sp
   0007 -  dp
   0008 -  ip_protocol
   0009 -  relative_name_p
   0010 -  admin_status
 */


int32_t nsrm_add_nwservice_bypass_rule_to_nschain_object(
        struct  nsrm_nwservice_bypass_rule_key*           key_info_p,
        int32_t number_of_config_params,
        struct  nsrm_nwservice_bypass_rule_config_info*   config_info_p
      );
         

int32_t nsrm_del_nwservice_bypass_rule_from_nschain_object(
        struct  nsrm_nwservice_bypass_rule_key*           key_info_p
        );

int32_t nsrm_modify_nwservice_bypass_rule(
        struct  nsrm_nwservice_bypass_rule_key*           key_info_p,
        int32_t number_of_config_params,
        struct  nsrm_nwservice_bypass_rule_config_info*   config_info_p
      );

int32_t nsrm_get_first_bypass_rules_from_nschain_object (
        struct   nsrm_nschain_object_key*     nschain_object_key_p,
        int32_t  number_of_bypass_rules_requested,
        int32_t* number_of_bypass_rules_returned_p,
        struct   nsrm_nwservice_bypass_rule_record* recs_p
        ); 

int32_t nsrm_get_next_bypass_rules_from_nschain_object (
        struct   nsrm_nwservice_bypass_rule_key*  relative_record_p,
        struct   nsrm_nschain_object_key*     nschain_object_key_p,
        int32_t  number_of_bypass_rules_requested,
        int32_t* number_of_bypass_rules_returnred_p,
        struct   nsrm_nwservice_bypass_rule_record* recs_p
        ); 

int32_t nsrm_get_exact_bypass_rule_from_nschain_object (
        struct   nsrm_nschain_object_key*     nschain_object_key_p,
        struct   nsrm_nwservice_bypass_rule_key*       key_info_p,
        struct   nsrm_nwservice_bypass_rule_record*  rec_p
        );
    
int32_t nsrm_attach_nwservice_to_bypass_rule(struct nsrm_bypass_rule_nwservice_key *key_info_p);
int32_t nsrm_detach_nwservice_from_bypass_rule(struct nsrm_bypass_rule_nwservice_key *key_info_p);
int32_t nsrm_get_first_nwservice_objects_from_bypass_rule(
                                struct   nsrm_nwservice_bypass_rule_key* key_info_p,
                                int32_t  number_of_nwservice_objects_requested,
                                int32_t* number_of_nwservice_objects_returned_p,
                                struct   nsrm_bypass_rule_nwservice_key *recs_p);
int32_t nsrm_get_next_nwservice_objects_from_bypass_rule(
                                struct   nsrm_nwservice_bypass_rule_key* key_info_p,
                                struct   nsrm_bypass_rule_nwservice_key* relative_node_p,
                                int32_t  number_of_nwservice_objects_requested,
                                int32_t* number_of_nwservice_objects_returned_p,
                                struct   nsrm_bypass_rule_nwservice_key *recs_p);
int32_t nsrm_get_exact_nwservice_objects_from_bypass_rule(
                                struct   nsrm_nwservice_bypass_rule_key* key_info_p,
                                struct   nsrm_bypass_rule_nwservice_key *key_p,
                                struct   nsrm_bypass_rule_nwservice_key *recs_p);

 
/* Attributes*/
int32_t nsrm_add_attribute_to_nwservice_bypass_rule(
          char*     nwservice_bypass_rule_name_p, 
          char*     nschain_object_name_p,
          struct    nsrm_attribute_name_value_pair *attribute_info_p);


int32_t nsrm_del_attribute_from_nwservice_bypass_rule(
           char*    nwservice_bypass_rule_name_p,
           char*     nschain_object_name_p,
           char*    attribute_name_p);


int32_t nsrm_modify_attribute_in_nwservice_bypass_rule(
          char*     nwservice_bypass_rule_name_p, 
          char*     nschain_object_name_p,
          struct    nsrm_attribute_name_value_pair *attribute_info_p);


int32_t nsrm_get_first_nwservice_bypass_rule_attribute(
          char*  nwservice_bypass_rule_name_p, 
          char*  nschain_object_name_p,
          struct nsrm_attribute_name_value_output_info *attribute_output_p);
      

int32_t nsrm_get_next_nwservice_bypass_rule_attribute(
          char*  nwservice_bypass_rule_name_p,  
          char*  nschain_object_name_p,
          char*  current_attribute_name_p,
          struct nsrm_attribute_name_value_output_info *attribute_output_p);

int32_t nsrm_get_exact_nwservice_bypass_rule_attribute(
            char*   nwservice_bypass_rule_name_p,
            char*  nschain_object_name_p,
            char*   attribute_name_p,
            struct  nsrm_attribute_name_value_output_info *attribute_output_p
          );  
/* Views */

int32_t nsrm_create_nwservice_bypass_rule_view(char* view_p);

int32_t nsrm_get_first_nwservice_bypass_rule_view_entries(
            char*     view_name_p,
            int32_t   number_of_nwservice_bypass_rules_requested,
            int32_t*  number_of_nwservice_bypass_rules_returned_p,
            struct    nsrm_nwservice_bypass_rule_record** recs_p
          );
 
int32_t nsrm_get_next_nwchain_bypass_rule_view_entries(
            char*     view_name_p,
            struct    nsrm_nwservice_bypass_rule_key* relative_record_p, 
            int32_t   number_of_nwservice_bypass_rules_requested,
            int32_t*  number_of_nwservice_bypass_rules_returned_p,
            struct    nsrm_nwservice_bypass_rule_record** recs_p
          );

/* Notifications */
struct nsrm_nwservice_bypass_rule_notification
{
  char*     nschain_object_name_p;
  uint64_t  nschain_object_handle;

  char*     nwservice_bypass_rule_name_p;
  uint64_t  nwservice_bypass_rule_handle;
};

struct nsrm_nwservice_bypass_rule_attribute_notification
{
  char*     nwservice_bypass_rule_name_p;
  uint64_t  nwservice_bypass_rule_handle;

  char*     bypass_rule_attribute_name_p;
  char*     bypass_rule_attribute_value_p;
};

union nsrm_nwservice_bypass_rule_notification_data
{
  struct nsrm_nwservice_bypass_rule_notification add_del;
  struct nsrm_nwservice_bypass_rule_attribute_notification attribute;
};

/*Callback for notifications*/
typedef void(*nsrm_nwservice_bypass_rule_notifier)
      (uint8_t notification_type,
       uint64_t bypass_rule_handle,
       union nsrm_nwservice_bypass_rule_notification_data notification_data,
       void *callback_arg1,
       void *callback_arg2);

/* Registration API */
int32_t nsrm_register_nwservice_bypass_rule_notifications( 
            uint8_t notification_type,
            nsrm_nwservice_bypass_rule_notifier  bypass_rule_notifier,
            void* callback_arg1,
            void* callback_arg2); 


enum nwservice_bypass_rule_notification_type
{
  NSRM_NWSERVICE_BYPASS_RULE_ALL,
  NSRM_NWSERVICE_BYPASS_RULE_ADDED_TO_NSCHAIN_OBJECT,
  NSRM_NWSERVICE_BYPASS_RULE_DELETED_FROM_NSCHAIN_OBJECT,
  NSRM_NWSERVICE_BYPASS_RULE_ATTRIBUTE_ADDED,
  NSRM_NWSERVICE_BYPASS_RULE_ATTRIBUTE_DELETED
};

/* ----------------------------------------------------------------*/

/* 4. Network service chain set */

struct nsrm_nschainset_object_key
{
  char* name_p;
  char* tenant_name_p;
};

struct nsrm_nschainset_object_config_info
{
  uint32_t field_id;
  union
  {
    uint8_t nschainset_type;
    uint8_t admin_status_e;
  }value;
};

struct nsrm_nschainset_object_record
{
  struct nsrm_nschainset_object_key          key;
  struct nsrm_nschainset_object_config_info*  info;
};

#define NSRM_MAX_NSCHAINSET_CONFIG_PARAMETERS 2

/* Field ID's defined for this database: 
 *  0001 - nschainset_type
 *  0002 - admin_status
 *    */

int32_t nsrm_add_nschainset_object(
        int32_t number_of_config_params,
        struct  nsrm_nschainset_object_key*           key_info_p,
        struct  nsrm_nschainset_object_config_info*   config_info_p
      );

int32_t nsrm_del_nschainset_object(
        struct  nsrm_nschainset_object_key*           key_info_p
        );


int32_t nsrm_modify_nschainset_object(
        struct  nsrm_nschainset_object_key*           key_info_p,
        int32_t number_of_config_params,
        struct  nsrm_nschainset_object_config_info*   config_info_p
      );


/* Allowed only if all the rules in the chain set are deleted */

/* This structure is exposed to Configuration Application like Open Stack */
int32_t nsrm_get_first_nschainset_objects(
        int32_t  number_of_nschainset_objects_requested,
        int32_t* number_of_nschainset_objects_returned_p,
        struct   nsrm_nschainset_object_record* recs_p
        );

int32_t nsrm_get_next_nschainset_objects(
        struct   nsrm_nschainset_object_key*   relative_record_key_p,
        int32_t  number_of_nschainset_objects_requested,
        int32_t* number_of_nschainset_objects_returned_p,
        struct   nsrm_nschainset_object_record* recs_p);

int32_t nsrm_get_exact_nschainset_object(
        struct   nsrm_nschainset_object_key*   key_info_p,
        struct   nsrm_nschainset_object_record*   rec_p
     );


/* Attributes */

int32_t nsrm_add_attribute_to_nschainset_object(
          char*     nschainset_object_name_p,     
          struct    nsrm_attribute_name_value_pair *attribute_info_p);

int32_t nsrm_del_attribute_from_nschainset_object(
           char*    nschainset_object_name_p, 
           char* attribute_name_p);

int32_t nsrm_modify_attribute_in_nschainset_object(
          char*     nschainset_object_name_p,     
          struct    nsrm_attribute_name_value_pair *attribute_info_p);

int32_t nsrm_get_first_nschainset_object_attribute(
          char*  nschainset_object_name_p, 
          struct nsrm_attribute_name_value_output_info *attribute_output_p);
      
int32_t nsrm_get_next_nschainset_object_attribute(
          char*  nschainset_object_name_p, 
          char*  current_attribute_name_p,
          struct nsrm_attribute_name_value_output_info *attribute_output_p);

int32_t nsrm_get_exact_nschainset_object_attribute(
            char* nschainset_object_name_p,
            char*   attribute_name_p,
            struct  nsrm_attribute_name_value_output_info *attribute_output_p
          );

/* Views */
int32_t nsrm_create_nschainset_object_view(char* view_p);

int32_t nsrm_get_first_nschainset_object_view_entries(
            char*     view_name_p,
            int32_t   number_of_nschainset_objects_requested,
            int32_t*  number_of_nschainset_objects_returned_p,
            struct    nsrm_nschainset_object_record* recs_p
          );

int32_t nsrm_get_next_nschainset_object_view_entries(
            char*    view_name_p,
            struct   nsrm_nschainset_object_key* relative_record_key_p,
            int32_t  number_of_nschainset_objects_requested,
            int32_t* number_of_nschainset_objects_returned_p,
            struct   nsrm_nschainset_object_record* recs_p
          );

/* Notifications */

struct nsrm_nschainset_object_notification
{
  char*     nschainset_object_name_p;
  uint64_t  nschainset_object_handle;
};

struct nsrm_nschainset_object_attribute_notification
{
  char*     nschainset_object_name_p;
  uint64_t  nschainset_object_handle;

  char*     nschainset_attribute_name_p;
  char*     nschainset_attribute_value_p;
};

union nsrm_nschainset_object_notification_data
{
  struct nsrm_nschainset_object_notification add_del;
  struct nsrm_nschainset_object_attribute_notification attribute;
};

/*Callback for notifications*/
typedef void(*nsrm_nschainset_object_notifier)
         (uint8_t notification_type,
          uint64_t nschainset_handle,
          union nsrm_nschainset_object_notification_data  notification_data,
          void *callback_arg1,
          void *callback_arg2);

/* Registration API */
int32_t nsrm_register_nschainset_object_notifications( 
            uint8_t notification_type,
            nsrm_nschainset_object_notifier  nschainset_notifier,
            void* callback_arg1,
            void* callback_arg2); 

enum nschainset_object_notification_type
{
  NSRM_NSCHAIN_SET_OBJECT_ALL = 0,
  NSRM_NSCHAIN_SET_OBJECT_ADDED,
  NSRM_NSCHAIN_SET_OBJECT_DELETED,
  NSRM_NSCHAIN_SET_OBJECT_ATTRIBUTE_ADDED,
  NSRM_NSCHAIN_SET_OBJECT_ATTRIBUTE_DELETED
};

/* ------------------------------------------------------------------------- */

/* 5. nschain_selection_rule */
struct nsrm_nschain_selection_rule_key
{
  char*   name_p;
  char*   tenant_name_p;
  char*   nschainset_object_name_p;
  uint8_t location_e;
};

struct nsrm_nschain_selection_rule_config_info
{
  uint32_t field_id;
  union
  {
    struct nsrm_mac src_mac;
    struct nsrm_mac dst_mac;
    struct nsrm_ethtype eth_type;
    struct nsrm_ip   sip;
    struct nsrm_ip   dip;
    struct nsrm_port sp;
    struct nsrm_port dp;
    uint16_t ip_protocol;

    char*   nschain_object_name_p;
    char*   relative_name_p;
    uint8_t admin_status_e;
  }value;
};
#define NSRM_MAX_SELECTION_RULE_CONFIG_PARAMETERS  11

/* Field ID's defined for this database: 
   0001 -  src_mac 
   0002 -  dst_mac 
   0003 -  eth_type
   0004 -  sip
   0005 -  dip
   0006 -  sp
   0007 -  dp
   0008 -  ip_protocol
   0009 -  nschain_object_name
   0010 -  relative_name_p
   0011 -  admin_status
 */

struct nsrm_nschain_selection_rule_record
{
  struct nsrm_nschain_selection_rule_key           key;
  struct nsrm_nschain_selection_rule_config_info*   info;
};

int32_t nsrm_add_nschain_selection_rule_to_nschainset(
        struct  nsrm_nschain_selection_rule_key*           key_info_p,
        int32_t number_of_config_params,
        struct  nsrm_nschain_selection_rule_config_info*   config_info_p
      );
 

int32_t nsrm_del_nschain_selection_rule_from_nschainset(
        struct  nsrm_nschain_selection_rule_key*           key_info_p
        );

int32_t nsrm_modify_nschain_selection_rule(
        struct  nsrm_nschain_selection_rule_key*           key_info_p,
        int32_t number_of_config_params,
        struct  nsrm_nschain_selection_rule_config_info*   config_info_p
      );
int32_t nsrm_get_first_selection_rules_from_nschainset_object (
        struct   nsrm_nschainset_object_key*     nchainset_object_key_p,
        int32_t  number_of_selection_rules_requested,
        int32_t* number_of_selection_rules_returned_p,
        struct   nsrm_nschain_selection_rule_record*  recs_p
        ); 
int32_t nsrm_get_next_selection_rules_from_nschainset_object (
        struct   nsrm_nschainset_object_key*     nschainset_object_key_p,
        struct   nsrm_nschain_selection_rule_key*  relative_record_p,
        int32_t  number_of_selection_rules_requested,
        int32_t* number_of_selection_rules_returned_p,
        struct   nsrm_nschain_selection_rule_record* recs_p
        );

int32_t nsrm_get_exact_selection_rule_from_nschainset_object(
        struct   nsrm_nschainset_object_key*     nschainset_object_key_p,
        struct   nsrm_nschain_selection_rule_key       *key_info_p,
        struct   nsrm_nschain_selection_rule_record  *recs_p
        );

/* Attributes*/

int32_t nsrm_add_attribute_to_nschain_selection_rule(
          char*     nschain_selection_rule_name_p, 
          char*     nschainset_object_name_p,
          struct    nsrm_attribute_name_value_pair *attribute_info_p);

int32_t nsrm_del_attribute_from_nschain_selection_rule(
           char*    nschain_selection_rule_name_p,
           char*     nschainset_object_name_p,
           char*    attribute_name_p);


int32_t nsrm_modify_attribute_in_nschain_selection_rule(
          char*     nschain_selection_rule_name_p, 
          char*     nschainset_object_name_p,
          struct    nsrm_attribute_name_value_pair *attribute_info_p);


int32_t nsrm_get_first_selection_rule_attribute(
          char*   nschain_selection_rule_name_p,
          char*   nschainset_object_name_p,
          struct  nsrm_attribute_name_value_output_info *attribute_output_p);

int32_t nsrm_get_next_selection_rule_attribute(
          char*   nschain_selection_rule_name_p,
          char*   nschainset_object_name_p,
          char*   current_attribute_name_p,
          struct  nsrm_attribute_name_value_output_info *attribute_output_p);

int32_t nsrm_get_exact_selection_rule_attribute(
          char*  nschain_selection_rule_name_p,
          char*  nschainset_object_name_p,
          char*  attribute_name_p,
          struct nsrm_attribute_name_value_output_info *attribute_output_p
          );

/* Views */
int32_t nsrm_create_nschain_selection_rule_view(char* view_p);

int32_t nsrm_get_first_nschain_selection_rule_view_entries(
            char*     view_name_p,
            int32_t   number_of_nschain_selection_rules_requested,
            int32_t*  number_of_nschain_selection_rules_returned_p,
            struct    nsrm_nschain_selection_rule_record* recs_p
          );

int32_t nsrm_get_next_nschain_selection_rule_view_entries(
            char*     view_name_p,
            struct    nsrm_nschain_selection_rule_key* relative_record_p,
            int32_t   number_of_nschain_selection_rules_requested,
            int32_t*  number_of_nschain_selection_rules_returned_p,
            struct    nsrm_nschain_selection_rule_record* recs_p
          );

/* Notifications */
struct nsrm_nschain_selection_rule_notification
{
  char*     nschainset_object_name_p;
  uint64_t  nschainset_object_handle;

  char*     nschain_selection_rule_name_p;
  uint64_t  nschain_selection_rule_handle;
};

struct nsrm_nschain_selection_rule_attribute_notification
{
  char*     nschain_selection_rule_name_p;
  uint64_t  nschain_selection_rule_handle;

  char*     nschain_selection_rule_attribute_name_p;
  char*     nschain_selection_rule_attribute_value_p;
};

union nsrm_nschain_selection_rule_notification_data
{
  struct nsrm_nschain_selection_rule_notification add_del;
  struct nsrm_nschain_selection_rule_attribute_notification attribute;
};

/*Callback for notifications*/
typedef void(*nsrm_nschain_selection_rule_notifier)
         (uint8_t notification_type,
          uint64_t selection_rule_handle,
         union nsrm_nschain_selection_rule_notification_data  notification_data,
          void *callback_arg1,
          void *callback_arg2);

/* Registration API */
int32_t nsrm_register_nschain_selection_rule_notifications( 
            uint8_t notification_type,
            nsrm_nschain_selection_rule_notifier  nschain_selection_notifier,
            void* callback_arg1,
            void* callback_arg2); 

enum nschain_selection_rule_notification_type
{
  NSRM_NSCHAIN_SELECTION_RULE_ALL = 0,
  NSRM_NSCHAIN_SELECTION_RULE_ADDED_TO_NSCHAIN_SET_OBJECT,
  NSRM_NSCHAIN_SELECTION_RULE_DELETED_FROM_NSCHAIN_SET_OBJECT,
  NSRM_NSCHAIN_SELECTION_RULE_ATTRIBUTE_ADDED,
  NSRM_NSCHAIN_SELECTION_RULE_ATTRIBUTE_DELETED
};

/* --------------------------------------------------------------- */                                              

/*  6. l2nw_service_map Attaching - nschainset Objects to l2 networks. */ 

struct nsrm_l2nw_service_map_key
{
  char*  tenant_name_p;
  char*  map_name_p;
  char*  vn_name_p;
};

struct nsrm_l2nw_service_map_config_info
{
  uint32_t field_id;
  union
  {
    char*    nschainset_object_name_p;
    uint8_t  admin_status_e;
  }value;
};

struct nsrm_l2nw_service_map_record
{
  struct   nsrm_l2nw_service_map_key           key;
  struct   nsrm_l2nw_service_map_config_info*   info;
};

#define NSRM_MAX_L2NW_CONFIG_PARAMETERS 2

/* Field ID's defined for this database:  
   0001 -  nschainset_object_name 
   0002 -  vn_name
   0003 -  admin_status
 */

int32_t nsrm_add_l2nw_service_map_record(
        int32_t number_of_config_params,
        struct  nsrm_l2nw_service_map_key* key_info_p,
        struct  nsrm_l2nw_service_map_config_info* config_info_p
       );


int32_t nsrm_del_l2nw_service_map_record(
        struct  nsrm_l2nw_service_map_key* key_info_p
        );

int32_t nsrm_modify_l2nw_service_map_record(
        struct  nsrm_l2nw_service_map_key* key_info_p,
        int32_t number_of_config_params,
        struct  nsrm_l2nw_service_map_config_info* config_info_p
       );

int32_t nsrm_get_first_l2nw_service_maps(
        int32_t  number_of_l2_service_maps_requested, 
        int32_t* number_of_l2_service_maps_returned_p, 
        struct   nsrm_l2nw_service_map_record* recs_p 
        );


int32_t nsrm_get_next_l2nw_service_maps(
        struct   nsrm_l2nw_service_map_key* relative_record_key_p,
        int32_t  number_of_l2_service_maps_requested,
        int32_t* number_of_l2_service_maps_returned_p,
        struct   nsrm_l2nw_service_map_record* recs_p
        );

int32_t nsrm_get_exact_l2nw_service_map(
        struct  nsrm_l2nw_service_map_key* key_info_p,
        struct  nsrm_l2nw_service_map_record* rec_p
        );


/* Attributes */

int32_t nsrm_add_attribute_to_l2nw_service_map(
          char*     l2nw_service_map_name_p, 
          struct    nsrm_attribute_name_value_pair *attribute_info_p);

int32_t nsrm_del_attribute_from_l2nw_service_map (
           char*    l2nw_service_map_name_p, 
           char*    attribute_name_p);

int32_t nsrm_modify_attribute_in_l2nw_service_map(
          char*     l2nw_service_map_name_p, 
          struct    nsrm_attribute_name_value_pair *attribute_info_p);


int32_t nsrm_get_first_l2nw_service_map_attribute(
          char*  l2nw_service_map_name_p, 
          struct nsrm_attribute_name_value_output_info *attribute_output_p);
      

int32_t nsrm_get_next_l2nw_service_map_attribute(
          char*  l2nw_service_map_name_p, 
          char*  current_attribute_name_p,
          struct nsrm_attribute_name_value_output_info *attribute_output_p);


int32_t nsrm_get_exact_l2nw_service_map_attribute(
            char*  l2nw_service_map_name_p,
            char*   attribute_name_p,
            struct  nsrm_attribute_name_value_output_info *attribute_output_p
          );

/* Views */
int32_t nsrm_create_l2nw_service_map_view(char* view_p);

int32_t nsrm_get_first_l2nw_service_map_view_entries(
             char*    view_name_p,
             int32_t  number_of_l2_service_maps_requested,
             int32_t* number_of_l2_service_maps_returned_p,
             struct   nsrm_l2nw_service_map_record* recs_p
           );
 
int32_t nsrm_get_next_l2nw_service_map_view_entries(
            char*    view_name_p,
            struct   nsrm_l2nw_service_map_key* relative_record_key_p,
            int32_t  number_of_l2_service_maps_requested,
            int32_t* number_of_l2_service_maps_returned_p,
            struct   nsrm_l2nw_service_map_record* recs_p
          );

/* Notifications */
struct nsrm_l2nw_service_map_notification
{
  char*     l2nw_service_map_name_p;
  uint64_t  l2nw_service_map_handle;
  uint64_t  vn_handle;
  uint64_t  nschainset_object_handle;
};

struct nsrm_l2nw_service_map_attribute_notification
{
  char*     l2nw_service_map_name_p;
  uint64_t  l2nw_service_map_handle;

  char*     l2nw_service_map_attribute_name_p;
  char*     l2nw_service_map_attribute_value_p;
};

union nsrm_l2nw_service_map_notification_data
{
  struct nsrm_l2nw_service_map_notification add_del;
  struct nsrm_l2nw_service_map_attribute_notification attribute;
};

/*Callback for notifications*/
typedef void(*nsrm_l2nw_service_map_notifier)
         (uint8_t notification_type,
          uint64_t l2nw_service_map_handle,
          union nsrm_l2nw_service_map_notification_data  notification_data,
          void *callback_arg1,
          void *callback_arg2);

/* Registration API */
int32_t nsrm_register_l2nw_service_map_notifications( 
            uint8_t notification_type,
            nsrm_l2nw_service_map_notifier  l2nw_map_notifier,
            void* callback_arg1,
            void* callback_arg2); 

enum l2nw_service_map_notification_type
{
  NSRM_L2NW_SERVICE_MAP_ALL = 0,
  NSRM_L2NW_SERVICE_MAP_ADDED,
  NSRM_L2NW_SERVICE_MAP_DELETED,
  NSRM_L2NW_SERVICE_MAP_ATTRIBUTE_ADDED,
  NSRM_L2NW_SERVICE_MAP_ATTRIBUTE_DELETED
};

/* ---------------------------------------------------------------- */                                           

/*  7. L3nw_service_map - Attaching nschainset Objects to l3 networks. */

struct nsrm_l3nw_service_map_key
{
  char*   map_name_p;
  char*   tenant_name_p;
};


struct nsrm_l3nw_service_map_config_info
{
  uint32_t field_id;
  union
  {
    char*    nschainset_object_name_p;
    char**   vn_name_p_p;
    uint8_t  admin_status_e;
  }value;
};

struct nsrm_l3nw_service_map_record
{
  struct   nsrm_l3nw_service_map_key           key;
  struct   nsrm_l3nw_service_map_config_info*  info;
};

#define NSRM_MAX_L3NW_CONFIG_PARAMETERS 3

/* Field ID's defined for this database:  
   0001 -  nschainset_object_name 
   0002 -  vn_name
   0003 -  admin_status
 */
int32_t nsrm_add_l3nw_service_map_record(
        struct  nsrm_l3nw_service_map_key* key_info_p,
        int32_t number_of_config_params,
        struct  nsrm_l3nw_service_map_config_info* config_info_p
       );
int32_t nsrm_del_l3nw_service_map_record(
        struct  nsrm_l3nw_service_map_key*  key_info_p
        );
int32_t nsrm_modify_l3nw_service_map_record(
        struct  nsrm_l3nw_service_map_key*  key_info_p,
        int32_t number_of_config_params,
        struct  nsrm_l3nw_service_map_config_info*  config_info_p
       );

int32_t nsrm_get_first_l3nw_service_maps(
        int32_t  number_of_l3_service_maps_requested, 
        int32_t* number_of_l3_service_maps_returned_p, 
        struct   nsrm_l3nw_service_map_record* recs_p 
        );
int32_t nsrm_get_next_l3nw_service_maps(
        struct   nsrm_l3nw_service_map_key*  relative_record_key_p,
        int32_t  number_of_l3_service_maps_requested,
        int32_t* number_of_l3_service_maps_returned_p,
        struct   nsrm_l3nw_service_map_record* recs_p 
        );
int32_t nsrm_get_exact_l3nw_service_map(
        struct nsrm_l3nw_service_map_key*  key_info_p,
        struct  nsrm_l3nw_service_map_record* rec_p
        );


int32_t nsrm_add_attribute_to_l3nw_service_map(
          char*     l3nw_service_map_name_p, 
          struct    nsrm_attribute_name_value_pair *attribute_info_p);

int32_t nsrm_del_attribute_from_l3nw_service_map (
           char*    l3nw_service_map_name_p, 
           char*     attribute_name_p);

int32_t nsrm_modify_attribute_in_l3nw_service_map(
          char*     l3nw_service_map_name_p, 
          struct    nsrm_attribute_name_value_pair *attribute_info_p);


int32_t nsrm_get_l3nw_service_map_first_attribute(
          char*    l3nw_service_map_name_p, 
          struct   nsrm_attribute_name_value_output_info *attribute_output_p);
      

int32_t nsrm_get_l3nw_service_map_next_attribute(
          char*    l3nw_service_map_name_p, 
          char*    current_attribute_name_p,
          struct   nsrm_attribute_name_value_output_info *attribute_output_p);

int32_t nsrm_get_exact_l3nw_service_map_attribute(
            char*   l3nw_service_map_name_p,
            char*   attribute_name_p,
            struct  nsrm_attribute_name_value_output_info *attribute_output_p
           );

/* Views */
int32_t nsrm_create_l3nw_service_map_view(char* view_p);

int32_t nsrm_get_first_l3nw_service_map_view_entries(
              char*     view_name_p,
              int32_t   number_of_l3_service_maps_requested,
              int32_t*  number_of_l3_service_maps_returned_p,
              struct    nsrm_l3nw_service_map_record* recs_p
            );
 
int32_t nsrm_get_next_l3nw_service_map_view_entries(
            char*    view_name_p,
            struct   nsrm_l3nw_service_map_key*  relative_record_key_p,
            int32_t  number_of_l3_service_maps_requested,
            int32_t* number_of_l3_service_maps_returned_p,
            struct   nsrm_l3nw_service_map_record* recs_p
          );

/* Notifications */
struct   nsrm_l3nw_service_map_notification
{
  char*     l3nw_service_map_name_p;
  uint64_t  l3nw_service_map_handle;
};

struct   nsrm_l3nw_service_map_attribute_notification
{
  char*     l3nw_service_map_name_p;
  uint64_t  l3nw_service_map_handle;

  char*     l3nw_service_map_attribute_name_p;
  char*     l3nw_service_map_attribute_value_p;
};

union nsrm_l3nw_service_map_notification_data
{
  struct   nsrm_l3nw_service_map_notification add_del;
  struct   nsrm_l3nw_service_map_attribute_notification attribute;
};

/*Callback for notifications*/
typedef void(*nsrm_l3nw_service_map_notifier)
         (uint8_t notification_type,
         uint32_t l3nw_service_map_handle,
         union nsrm_l3nw_service_map_notification_data  notification_data,
         void *callback_arg1,
         void *callback_arg2);

/* Registration API */
int32_t nsrm_register_l3nw_service_map_notifications( 
            uint8_t notification_type,
            nsrm_l3nw_service_map_notifier  l3nw_map_notifier,
            void* callback_arg1,
            void* callback_arg2); 

enum l3nw_service_map_notification_type
{ 
    NSRM_L3NW_SERVICE_MAP_ALL,
    NSRM_L3NW_SERVICE_MAP_ADDED,
    NSRM_L3NW_SERVICE_MAP_DELETED,
    NSRM_L3NW_SERVICE_MAP_ATTRIBUTE_ADDED,
    NSRM_L3NW_SERVICE_MAP_ATTRIBUTE_DELETED  
};






