#include "dprm.h"
#define DPRM_SWITCH_FIRST_NOTIFICATION_TYPE DPRM_SWITCH_ALL_NOTIFICATIONS
#define DPRM_SWITCH_LAST_NOTIFICATION_TYPE  DPRM_SWITCH_DATAPATH_DETACH

#define DPRM_DOMAIN_FIRST_NOTIFICATION_TYPE DPRM_DOMAIN_ALL_NOTIFICATIONS
#define DPRM_DOMAIN_LAST_NOTIFICATION_TYPE  DPRM_DOMAIN_DATAPATH_DETACH

#define DPRM_DATAPATH_FIRST_NOTIFICATION_TYPE DPRM_DATAPATH_ALL_NOTIFICATIONS
#define DPRM_DATAPATH_LAST_NOTIFICATION_TYPE  DPRM_DATAPATH_PORT_DESC_UPDATE

#define DPRM_COMPUTE_HASH(param1,param2,parm3,parm4,param5,hashkey) \
                          DPRM_DOBBS_WORDS4(param1,param2,param3,param4,0xa2956174,param5,hashkey) 

#define SWNODE_SWTBL_OFFSET offsetof(struct dprm_switch_entry,sw_tbl_link);
#define DMNODE_DMTBL_OFFSET offsetof(struct dprm_domain_entry,domain_tbl_link);
#define OFTABLENODE_OFTABLETBL_OFFSET offsetof(struct dprm_oftable_entry,oftable_tbl_link);

#define DPNODE_DPTBL_OFFSET offsetof(struct dprm_datapath_entry,dp_tbl_link);
#define DPNODE_SWTBL_OFFSET offsetof(struct dprm_datapath_entry,sw_tbl_link);
#define DPNODE_DMTBL_OFFSET offsetof(struct dprm_datapath_entry,domain_tbl_link);
#define PORTNODE_PORTTBL_OFFSET offsetof(struct dprm_port_entry,port_tbl_link);

/*Application nodes  that register for events memory pools*/
#define CNTLR_MAX_APP_NODES_IN_DOMAIN  10
#define CNTLR_MAX_NUM_APP_NODES_IN_SYSTEM (DPRM_MAX_DOMAIN_ENTRIES* CNTLR_MAX_APP_NODES_IN_DOMAIN)
#define CNTLR_STATIC_APP_NODES_IN_SYSTEM (DPRM_MAX_DOMAIN_STATIC_ENTRIES * CNTLR_MAX_APP_NODES_IN_DOMAIN)


#define OF_DP_DESCR_LEN            DESC_STR_LEN
#define OF_DP_SERIAL_NUM_STR_LEN   SERIAL_NUM_LEN
#define dprm_debug printf
#if 1 
/* MAXIMUM Number of async messages or events supported*/
#define OF_FIRST_ASYNC_EVENT      OF_ASYNC_MSG_PACKET_IN_EVENT /* ASSUMPTION  - This value is always 1*/
#define OF_LAST_ASYNC_EVENT       OF_ASYNC_MSG_EXPERIMENTER_EVENT
#define OF_MAX_NUMBER_ASYNC_MSGS (OF_LAST_ASYNC_EVENT - OF_FIRST_ASYNC_EVENT) + 1
#endif

/** WARNING  We are depnding on this macro for handing async events freceived
 * from data_path. Current OFPT_PACKET_IN is first event type.
 * but there is a posiblity of adding new async message type before packet_in
 *
 * We are depnding on this to have better peformance, but we need better logic
 **/
#define OF_FIRST_ASYNC_MSG_TYPE_ID OFPT_PACKET_IN

/* Deatils of memory primitives maintained by open flow async message*/
struct of_async_msg_mem_primitive
{
   uint8_t  is_configured;
   uint16_t head_room_size; /** Head room reserved for the async messessage*/
   alloc_func msg_alloc_cbk; /** Memory allocation callback function used to
                                 allocate message buffer*/
};

#if 0
/* Applications detils for the given async message event. Appliations might register
 * to revieve events with priority. 
 **/
struct async_event_info
{
   uint8_t   type; /** Message or event type*/

   /** Details of each application registered, list of nodes type 'struct of_app_hook_info'
   *  values*/
   of_list_t app_list;
};
#endif
#define CNTLR_FIRST_EVENT_TYPE OPEN_CONN_FROM_DP_EVENT

#define CNTLR_LAST_EVENT_TYPE  DP_DETACH_EVENT

#define CNTLR_MAX_EVENTS ((CNTLR_LAST_EVENT_TYPE - CNTLR_FIRST_EVENT_TYPE) + 1)

/* Details of application registering for the given event*/
struct domain_app_event_info
{
  /** Message or event type*/
   uint8_t   type; 

   /** Details of each application registered, list of nodes type 'struct of_app_hook_info'
    *  values*/
   of_list_t app_list;
};

int32_t dprm_get_port_handle(uint64_t dp_handle,char* port_name_p,uint64_t* port_handle_p);
uint32_t dprm_get_hashval_byname(char* name_p,uint32_t no_of_buckets);
uint32_t dprm_get_hashval_byid(uint64_t dpid,uint32_t no_of_buckets);

unsigned int hashkey_func(void *pHashArgs,unsigned int ulBuckets);
unsigned int matchnode_func(void *pObjNode,unsigned int Arg1,unsigned int Arg2,unsigned int Arg3,
                            unsigned int Arg4,unsigned int Arg5,unsigned int Arg6,unsigned int Arg7);

void dprm_get_switch_mempoolentries(unsigned int* switch_entries_max,unsigned int* switch_static_entries);
void dprm_get_swnode_datapath_mempoolentries(uint32_t* swnode_dptable_entries_max, uint32_t* swnode_dptable_static_entries);
void dprm_get_dmnode_datapath_mempoolentries(uint32_t* dmnode_dptable_entries_max, uint32_t* dmnode_dptable_static_entries);
void dprm_get_dpnode_port_mempoolentries(uint32_t* port_entries_max, uint32_t* port_static_entries);

void dprm_free_swentry_rcu(struct rcu_head* node_p);
void dprm_free_swnode_dptbl_entry_rcu(struct rcu_head *datapath_entry_p);
void dprm_free_dmnode_dptbl_entry_rcu(struct rcu_head *datapath_entry_p);
void dprm_free_dmnode_oftbl_entry_rcu(struct rcu_head *oftable_entry_p);
void dprm_free_dpnode_porttbl_entry_rcu(struct rcu_head *port_entry_p);
void dprm_attributes_free_attribute_entry_rcu(struct rcu_head *attribute_entry_p);

struct dprm_oftable_entry;
struct dprm_port_entry;
struct dprm_datapath_entry;

/* Get internal table,port and datapath structures for controller to maintain run time information*/
int32_t get_domain_table_byname(uint64_t domain_handle,char* table_name_p,struct dprm_oftable_entry **table_info_p_p);
int32_t get_datapath_port_byhandle(uint64_t datapath_handle,uint64_t port_handle,struct dprm_port_entry** port_info_p_p);
int32_t get_datapath_byhandle(uint64_t datapath_handle,struct dprm_datapath_entry** datapath_info_p_p);
int32_t get_port_byname(struct dprm_datapath_entry* datapath_info_p,char* port_name_p,struct dprm_port_entry** port_info_p_p);
int32_t dprm_get_datapath_id_byhandle(uint64_t datapath_handle,uint64_t *dp_id);
int32_t dprm_get_datapath_multinamespaces(uint64_t dpid, uint8_t *multinamespaces);
int32_t dprm_get_datapath_namespace_count(uint64_t dpid, uint32_t *namespacescount);
int32_t dprm_get_datapath_port_namespace_count(uint64_t dpid, char *portname, uint32_t *namespacescount);
int32_t dprm_get_datapath_controller_role(uint64_t datapath_handle, uint8_t *controller_role);

/**************************************/

/* Only for testing purpose and called from dprmtest.c */
void get_switch(void);
void get_domain(void);
void get_table(uint64_t domain_handle );
void get_datapath(void);
void get_dp_port(uint64_t dp_handle);
#if 0
/**************************************/
struct dprm_oftable_entry
{
  struct    mchash_table_link oftable_tbl_link;
  struct    dprm_domain_entry *domain_entry_p;
  char      table_name[DPRM_MAX_LEN_OF_TABLE_NAME];
  uint8_t   table_id;
  struct    async_event_info event_info[OF_MAX_NUMBER_ASYNC_MSGS + 1];
  
  /* Fixed during config time whem the name based table is added */
  uint8_t   no_of_match_fields;
  struct    dprm_match_field* match_field_p;
 
  /* of list holding attributes added for this oftable*/
  uint32_t  number_of_attributes;
  of_list_t attributes;
  
  uint32_t  index;
  uint32_t  magic;
  uint64_t  oftable_handle;
  int32_t   heap_b;  
};  
#endif
struct dprm_port_entry
{
  struct    mchash_table_link port_tbl_link;
  struct    dprm_datapath_entry *datapath_entry_p;
  char      port_name[DPRM_MAX_PORT_NAME_LEN];
  uint32_t  port_id; 
  struct    ofl_port_desc_info port_info;
 
  /* of list holding attributes added for this port.*/
  uint32_t  number_of_attributes;
  of_list_t attributes;

  uint32_t  magic;
  uint32_t  index;
  uint64_t  port_handle;
  int32_t   heap_b;
  uint64_t  crm_port_handle;
};

struct dprm_switch_entry
{
  
  int xxxx;
  unsigned short vals1; 

  struct mchash_table_link sw_tbl_link;

  char name[DPRM_MAX_SWITCH_NAME_LEN];
  uint8_t switch_type;
  char*  switch_fqdn_p;
  struct ipv4_addr ipv4addr;
  struct ipv6_addr ipv6addr;
  
  /* Hash Table holding datapaths added to this switch. */
  uint32_t number_of_datapaths;
  struct mchash_table* dprm_swnode_dptable_p;  
  
  /* of list holding attributes added to this switch. */
  uint32_t number_of_attributes;
  of_list_t  attributes;
  
  uint32_t magic;
  uint32_t index;
  uint64_t switch_handle;  
  
  uint8_t heap_b;
};

 /**List of Applications registered to receive DPRM resource (sw,domain,datapath,table,port) related notifications
   *like addition,deletion,modification etc...Applications may regiser for a specfic notification or all the notifications
   *related to a specific resource(sw,domain etc..).Priority is not maintained.
   */

 struct dprm_notification_app_hook_info
 {
   /** to traverse next application registered for a dprm resource notifications */
   /** for RCU operation*/
   struct rcu_head  rcu_head;
   /** Application callback function to call*/
   void  *call_back;
   of_list_node_t  next_app;
   /** Application specific private info*/
   void  *cbk_arg1;
   /** Application specific private info*/
   void  *cbk_arg2;
   /** Boolean flag indicate whether this memory is allocated from heap or not*/
   uint8_t heap_b;
};
#define DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET offsetof(struct dprm_notification_app_hook_info,next_app)
struct dprm_resource_attribute_entry
{
  struct   rcu_head rcu_head;
  char     name[DPRM_MAX_ATTRIBUTE_NAME_LEN];
  of_list_node_t next_attribute;
//  char     value[DPRM_MAX_ATTRIBUTE_VALUE_LEN];
  uint8_t  heap_b;
  uint32_t number_of_attribute_values;
  of_list_t attribute_value_list;
};  
#define DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET offsetof(struct dprm_resource_attribute_entry, next_attribute)

#if 1
struct dprm_resource_attribute_value_entry
{
  struct   rcu_head rcu_head;
  char     value[DPRM_MAX_ATTRIBUTE_VALUE_LEN];
  of_list_node_t next_attribute;
  uint8_t  heap_b;
};  
#define DPRM_RESOURCE_ATTRIBUTE_VALUE_ENTRY_LISTNODE_OFFSET offsetof(struct dprm_resource_attribute_value_entry, next_attribute)
#endif
/*Details of applications registered to receive asynchronous messages from
 *datapath.Applications registers for interested events. Applications registeres
 *with different prioirity.
 **/
struct of_app_hook_info
{
   of_list_node_t      next_app; /** to traverse next application registered for the message type*/
   struct rcu_head     rcu_head; /** for RCU operation*/
   uint8_t             priority; /** Priority of the application to invoke, higher prioirty value
                                     application will be called first.*/
   void               *call_back; /** Application callback function to call*/
   void               *cbk_arg1; /** Application specific private info*/
   void               *cbk_arg2; /** Application specific private info*/
   uint8_t             is_heap; /** Boolean flag indicate whether this memory allocated in heap or not*/
};
#define OF_APP_HOOK_LISTNODE_OFFSET offsetof(struct of_app_hook_info, next_app)


/*MAXIMUM number of symmetric messages supported*/
#define OF_FIRST_SYMMMETRIC_MSG_EVENT OF_SYMMETRIC_MSG_HELLO /* ASSUMPTION  - This value is always 1*/
#define OF_LAST_SYMMMETRIC_MSG_EVENT OF_SYMMETRIC_MSG_ECHO_REQ
#define OF_MAX_NUMBER_SYM_MSGS (OF_LAST_SYMMMETRIC_MSG_EVENT-OF_FIRST_SYMMMETRIC_MSG_EVENT)+1

/* This maintains application callbacks  that registered to receive symmetric messges*/
struct symmetric_msg_info
{
   uint8_t type; /*Type of symmetrict message*/
   void    *callback;/*Callback function to recieve for the type of message
                       Only one callback function can be registered,
                       If more than one callback is registered new one will overwrites
                       curren one*/
   void   *cbk_arg1; /*Application spercific private that passed to callback*/
   void   *cbk_arg2; /*Application spercific private that passed to callback*/
};
#define CNTLR_MAX_TTP_NAME_LEN 64
struct dprm_domain_entry
{
  int xxxx;
  unsigned short vals1;
  struct mchash_table_link domain_tbl_link;
  cntlr_lock_t   lock;

  /** Name of the domain which must be unique among the domains */
  char name[DPRM_MAX_FORWARDING_DOMAIN_LEN+1];

  /** Name of the Table type pattern to which this domain belongs **/
  char ttp_name[CNTLR_MAX_TTP_NAME_LEN+1];

  /** Subject name used to Authenticate the remote datapath*/
  char expected_subject_name_for_authentication[DPRM_DOMAIN_MAX_X509_SUBJECT_LEN + 1];
#if 0
  /* Tables are added dynamically to this array which is indexed using table id */
  /* Both tables contain the same structure dprm_oftable_entry */
  struct  dprm_oftable_entry* table_p[DPRM_MAX_NUMBER_OF_TABLES];
  /* counter holding the number of id based tables added sofar */
  uint8_t number_of_tables_id_based;

  /* Hash Table holding oftables added to this domain based on table name. */
  struct   mchash_table* dprm_dmnode_oftable_p;
  /* counter holding the number of name based tables added sofar */
  uint8_t number_of_name_based_tables;
  /* mempool to hold table entries */
  void*  dprm_dmnode_oftable_mempool_g;
#endif

  /* Hash Table holding datapaths added to this domain. */
  uint32_t number_of_datapaths;
  struct   mchash_table* dprm_dmnode_dptable_p;
  /* of list holding attributes added to this domain. */
  uint32_t number_of_attributes;
  of_list_t  attributes;

  uint8_t  is_dflt_mem_configured;  /*Boolean flag indicate whether default memory primitive
                                     configured  or not*/

  /** Default async message primitive . It is used if no memory primitives registered for
   * given async message type */
  struct of_async_msg_mem_primitive   default_msg_mem_primitive;

  struct of_async_msg_mem_primitive   msg_type_mem_primitive[OF_MAX_NUMBER_ASYNC_MSGS+1];

  /**Details of application registering to receive asynchronous events*/
  struct domain_app_event_info   app_event_info[CNTLR_MAX_EVENTS+1];

  /**Details of application registering to receive sysnchronous events */
  struct symmetric_msg_info  symmetric_msg_handle[OF_MAX_NUMBER_SYM_MSGS+1];

  uint32_t magic;
  uint32_t index;
  uint64_t domain_handle;
  uint8_t  heap_b;
};

typedef struct mchash_table of_conn_table_t;

/** Maintains details deails of a channel created for the data_path
 * The data_path entry maintains list of all the channels including main
 * created for the data path, this is required to load balance 
 * of all the traffic sending to channel*/
typedef struct dprm_dp_channel_entry_s
{
    struct rcu_head                  rcu_head; /** for RCU operation*/
    struct dprm_dp_channel_entry_s  *next;/** next dp channel pointer*/
    struct dprm_dp_channel_entry_s  *prev; /** previous dp channel pointer*/
    uint32_t                         status; /** Connection status */
    uint32_t                         auxiliary_id; /* Auxiliary connection ID initiated from datapath*/
    uint64_t                         dp_handle;/*Handle of the datapath, the datapath to which this channel belongs*/
    struct dprm_datapath_entry      *datapath; /*Datapath to which this channels belongs*/
    cntlr_conn_table_t              *conn_table; /** Connection table to which this channel is added */
    cntlr_conn_node_saferef_t        conn_safe_ref; /** Safe reference of the channel opened by transport connection*/
    uint32_t thread_id;
} dprm_dp_channel_entry_t;


#define DP_BARRIER_STATE     TRUE
#define DP_NOT_BARRIER_STATE FALSE
struct barrier_state_msg_queue_node
{
   of_list_node_t             next_node;
   struct of_msg              *msg;
   struct dprm_datapath_entry *datapath;
   cntlr_conn_table_t         *conn_table;
   cntlr_conn_node_saferef_t   conn_safe_ref;
   void                       *cbk_fn;
   void                       *cbk_arg1;
   void                       *cbk_arg2;
};
#define BARRIER_STATE_MSG_QUEUE_NODE_LISTNODE_OFFSET offsetof(struct barrier_state_msg_queue_node, next_node)

struct dprm_datapath_entry
{
  int xxxx;
  unsigned short vals1;
  struct mchash_table_link dp_tbl_link;
  struct mchash_table_link sw_tbl_link;
  struct mchash_table_link domain_tbl_link;
 

  /** ID of the datapath */
  uint64_t dpid;

  /** Subject name used to authenticate the remote datapath */
  char expected_subject_name_for_authentication[DPRM_MAX_X509_SUBJECT_LEN + 1];
  char datapath_name[DPRM_MAX_DATAPATH_NAME_LEN + 1];

  /* Hash Table holding ports attached to this datapath. */
  uint32_t number_of_ports;
  struct   mchash_table* dprm_dpnode_porttable_p;

  /* of list holding attributes added to this datapath. */
  uint32_t number_of_attributes;
  of_list_t attributes;
  
  void*    dprm_dpnode_port_mempool_g;  
  
  uint64_t swnode_dp_output_handle_p;
  uint32_t swnode_dp_saferef_magic;
  uint32_t swnode_dp_saferef_index;

  uint64_t dmnode_dp_output_handle_p;
  uint32_t dmnode_dp_saferef_magic;
  uint32_t dmnode_dp_saferef_index;

  uint32_t sw_handle_magic;
  uint32_t sw_handle_index;

  uint32_t dm_handle_magic;
  uint32_t dm_handle_index;

  char     switch_name[DPRM_MAX_SWITCH_NAME_LEN + 1];  
  of_data_path_id_t              datapath_id; /** 64 bit Datapath Id  maintained at switch*/
  
  struct dprm_domain_entry      *domain;
  uint8_t                        controller_role; /** Controller Role w.r.t this data_path*/
  char                           mfr_desc[OF_DP_DESCR_LEN+1]; /** Switch manifacturing description*/
  char                           hw_desc[OF_DP_DESCR_LEN+1]; /** Switch hardware decription */
  char                           sw_desc[OF_DP_DESCR_LEN+1]; /** Switch software descrption*/
  char                           serial_num[OF_DP_SERIAL_NUM_STR_LEN+1]; /** Serial number of the switch*/
  char                           dp_desc[OF_DP_DESCR_LEN+1]; /** Datapath decription*/
  uint32_t                       no_of_buffers; /** Queue length maintained at data_path. This queue is used
                                                    to buffer messages before sending it to controller.*/
  uint32_t                       capabilities; /** Datapath capabilities , utitlity function to get details*/ 
  uint8_t                        no_of_tables; /** Number of table supported by the data_path*/
  void                          *tables; /*Pointer to tables available for the datpath, Depending on
                                           the number of tables received from datapath, memory will be 
                                            allocated for all table entries will be created*/
  of_list_t                      table_list; /** table list Data type dprm_dp_table_entry_t */
  of_list_t                      group_table; /** List of group entries maintained by datapath*/ 
  of_list_t                      meter_table; /** List of meter entries maintained by datapath*/ 
  of_list_t                      flow_table; /** List of group entries maintained by datapath*/ 
 //TBD struct ofp_table_features     *tables_features; 
  /*Queue Info */
  /*Meter Table*/
  uint8_t                        is_main_conn_present; /** Boolean flag indicate whehter main channel 
                                                           present or not */
  dprm_dp_channel_entry_t        main_channel; /* main channel info*/
  uint8_t                        curr_no_of_aux_chanels; /* Number of auxiliary channels opened by the 
                                                            data_path*/
  cntlr_lock_t                      aux_ch_list_lock; /** Lock used to manipulate auxiliary channels */
  dprm_dp_channel_entry_t       *aux_channels; /** Pointer to list of auxiliary channels, it is circular 
                                                           linked list */
  dprm_dp_channel_entry_t         *aux_chn_tail; /** Pointer to tail node auxiliary channels */ 
  dprm_dp_channel_entry_t         *curr_chn_used_2_q_msg;/** this is used for load balancing of msgs across channels*/
#if 0
  /*TBD, following might be not required*/
  uint8_t                 bSecEnabled;  
  of_DPCertInfo_t         *pCertInfo;

#endif
  uint8_t                        barrier_state; /**Boolean flag, TRUE indicates, barrier message has been sent to
                                                   data_path, it will be reset back once barrier reponse received*/
  of_list_t                      msg_queue_in_barrier_state; /** Queue used when datapath is in barrrier state ,
                                                                once barrier response is received  queued packets will be 
                                                                send to channel, it is of type struct barrier_state_msg_queue_node*/
  uint32_t                       magic;
  uint32_t                       index;
  uint64_t                       datapath_handle;

  uint8_t heap_b;
  uint8_t multi_namespaces;
};

typedef struct dprm_dp_table_entry_s
{
  of_list_node_t           next_table;
  struct rcu_head           rcu_head;
#define OF_DP_TABLE_ENTRY_INVALID  0
#define OF_DP_TABLE_ENTRY_VALID    1
#define OF_DP_TABLE_ENTRY_ACTIVE   2
  uint8_t                      status;
  char                         table_name[OFP_MAX_TABLE_NAME_LEN + 1];
  uint8_t                      dp_table_id;
  uint8_t                      domain_table_id;
  struct dprm_datapath_entry  *datapath;
  uint32_t                     max_no_of_entries;
  // TBD of_Table_Features_t          features;
  uint8_t                      no_of_match_fileds;
  // TBDof_DP_Table_Matchfields_t   *match_fields;
  uint16_t                     match_field_buf_len;
  void                        *matchfield_mempool;
} dprm_dp_table_entry_t;

#define DPRM_DP_TABLE_ENTRY_OFFSET offsetof(dprm_dp_table_entry_t, next_table)

enum of_inst_set_flags {
    OF_WRITE_ACTION_IN_PROG = 1 << 0,
    OF_APPLY_ACTION_IN_PROG = 1 << 1,
};


#define DPRM_DOBBS_MIX(a,b,c) \
{ \
      a -= b; a -= c; a ^= (c>>13); \
      b -= c; b -= a; b ^= (a<<8); \
      c -= a; c -= b; c ^= (b>>13); \
      a -= b; a -= c; a ^= (c>>12);  \
      b -= c; b -= a; b ^= (a<<16); \
      c -= a; c -= b; c ^= (b>>5); \
      a -= b; a -= c; a ^= (c>>3);  \
      b -= c; b -= a; b ^= (a<<10); \
      c -= a; c -= b; c ^= (b>>15); \
}
#define DPRM_DOBBS_WORDS4(word_a, word_b, word_c, word_d, initval, \
                                 hashmask, hash) \
{ \
      register unsigned long temp_a, temp_b, temp_c; \
      temp_a = temp_b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */\
      temp_c = initval;         /* random value*/ \
    \
      temp_a += word_a; \
      temp_b += word_b; \
      temp_c += word_c; \
    \
      DPRM_DOBBS_MIX(temp_a, temp_b, temp_c); \
    \
      temp_a += word_d; \
      DPRM_DOBBS_MIX(temp_a, temp_b, temp_c); \
    \
      hash = temp_c & (hashmask-1); \
}

#define DPRM_COMPUTE_HASH(param1,param2,parm3,parm4,param5,hashkey) \
                                    DPRM_DOBBS_WORDS4(param1,param2,param3,param4,0xa2956174,param5,hashkey) 



int32_t get_domain_byhandle(uint64_t domain_handle,struct dprm_domain_entry** domain_info_p_p);
int32_t cntlr_register_asynchronous_event_hook(uint64_t               domain_handle,
    uint8_t                event_type,
    uint8_t                priority,
    async_event_cbk_fn    call_back,
    void                  *cbk_arg1,
    void                  *cbk_arg2);

int32_t dprm_init(void);
int32_t get_domain_oftable_byhandle(uint64_t domain_handle,uint64_t oftable_handle_p,
                                    struct dprm_oftable_entry **table_info_p_p);
int32_t add_domain_oftable_to_idbased_list(uint64_t dm_handle,uint8_t table_id,
                                     struct dprm_oftable_entry *table_info_p);
int32_t perform_tests_on_idbased_table(void);



/*Namespaces */
struct dprm_namespace_entry
{
  int xxxx;
  unsigned short vals1; 

  struct mchash_table_link ns_tbl_link;
  char name[DPRM_MAX_SWITCH_NAME_LEN];
  
  uint32_t magic;
  uint32_t index;
  uint64_t namespace_handle;  
  
  uint8_t heap_b;
};
#define NSNODE_NSTBL_OFFSET offsetof(struct dprm_namespace_entry,ns_tbl_link)
#define RCUNODE_OFFSET_IN_MCHASH_TBLLNK offsetof( struct mchash_table_link, rcu_head)

int32_t dprm_get_logical_switch_handle(uint64_t switch_handle,
                                       char *datapath_name,
                                       uint64_t *datapath_handle_p);
int32_t dprm_send_port_update_events( struct    dprm_datapath_entry* datapath_info_p,
               struct    dprm_port_entry* dprm_port_p);

void dp_port_notifications_cbk(uint32_t dprm_notification_type,
                               uint64_t datapath_handle,
                               struct   dprm_datapath_notification_data ,
                               void     *callback_arg1,
                               void     *callback_arg2);

int32_t get_switch_byhandle(uint64_t switch_handle,struct dprm_switch_entry** switch_info_p_p);
