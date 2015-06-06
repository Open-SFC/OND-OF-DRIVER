/* Network Service resource manager local definitions header file */
/** Macros */
#define nsrm_debug printf

typedef pthread_mutex_t cntlr_lock_t;
typedef unsigned char uchar8_t;

#define NSRM_MAX_VIEW_DATABASE 10


#define NSRM_DOBBS_MIX(a,b,c)\
{ \
    a -= b; a -= c; a ^= (c>>13);\
    b -= c; b -= a; b ^= (a<<8);\
    c -= a; c -= b; c ^= (b>>13);\
    a -= b; a -= c; a ^= (c>>12);\
    b -= c; b -= a; b ^= (a<<16);\
    c -= a; c -= b; c ^= (b>>5);\
    a -= b; a -= c; a ^= (c>>3);\
    b -= c; b -= a; b ^= (a<<10);\
    c -= a; c -= b; c ^= (b>>15);\
}

#define NSRM_DOBBS_WORDS4(word_a, word_b, word_c, word_d, initval, \
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
    NSRM_DOBBS_MIX(temp_a, temp_b, temp_c); \
    \
    temp_a += word_d; \
    NSRM_DOBBS_MIX(temp_a, temp_b, temp_c); \
    \
    hash = temp_c & (hashmask-1); \
}
#define NSRM_COMPUTE_HASH(param1,param2,parm3,parm4,param5,hashkey) \
        NSRM_DOBBS_WORDS4(param1,param2,param3,param4,0xa2956174,param5,hashkey) 

#define NSRM_MAX_NAME_LEN     128

struct nsrm_resource_attribute_entry
{
  of_list_node_t next_attribute;
  struct   rcu_head rcu_head;
  uint8_t  heap_b;
  char*    name;
  char*    value;
};

#define NSRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET offsetof(struct nsrm_resource_attribute_entry, next_attribute)




struct nsrm_notification_app_hook_info
{
  /** to traverse next application registered for a NSRM resource notifications */
  of_list_node_t  next_app; /** for RCU operation*/
  struct rcu_head  rcu_head;
  uint8_t priority; /** Priority of the application to invoke, higher prioirty value
                    *application will be called first.*/

  /** Application callback function to call*/
  void  *call_back;
  /** Application specific private info*/
  void  *cbk_arg1;
  /** Application specific private info*/
  void  *cbk_arg2;
  /** Boolean flag indicate whether this memory is allocated from heap or not*/
  uint8_t heap_b;
};

#define NSRM_NOTIFICATION_APP_HOOK_OFFSET   offsetof(struct nsrm_notification_app_hook_info, next_app)


int32_t nsrm_init(void);




#define NSRM_NWSERVICE_OBJECT_TBL_OFFSET offsetof(struct nsrm_nwservice_object,nwservice_object_tbl_link)
#define NSRM_NSCHAIN_OBJECT_TBL_OFFSET   offsetof(struct nsrm_nschain_object,nschain_object_tbl_link)
#define NSRM_NSCHAIN_NWSERVICE_OBJECT_TBL_OFFSET   offsetof(struct nschain_nwservice_object,nschain_nwservice_object_tbl_link)
#define NSRM_NSCHAINSET_OBJECT_TBL_OFFSET   offsetof(struct nsrm_nschainset_object,nschainset_object_tbl_link)
#define NSRM_L2NW_SERVICE_MAP_TBL_OFFSET   offsetof(struct nsrm_l2nw_service_map,l2nw_service_map_tbl_link)

#define NSRM_NWSERVICE_INSTANCE_TBL_OFFSET   offsetof(struct nwservice_instance,nwservice_instance_tbl_link)


#define NSRM_SELECTION_RULE_TBL_OFFSET   offsetof(struct nsrm_nschain_selection_rule,selection_rule_tbl_link)


#define NSRM_BYPASS_RULE_TBL_OFFSET   offsetof(struct nsrm_nwservice_bypass_rule,bypass_rule_tbl_link)
#define NSRM_NWSERVICE_SKIP_LISTNODE_OFFSET  offsetof(struct nwservice_object_skip_list,next_node)


#define NSRM_NWSERVICE_OBJECT_FIRST_NOTIFICATION_TYPE      NSRM_NWSERVICE_OBJECT_ALL          
#define NSRM_NWSERVICE_OBJECT_LAST_NOTIFICATION_TYPE       NSRM_NWSERVICE_OBJECT_ATTRIBUTE_DELETED          

#define NSRM_NSCHAIN_OBJECT_FIRST_NOTIFICATION_TYPE        NSRM_NSCHAIN_OBJECT_ALL          
#define NSRM_NSCHAIN_OBJECT_LAST_NOTIFICATION_TYPE         NSRM_NSCHAIN_OBJECT_ATTRIBUTE_DELETED          
#define NSRM_NSCHAINSET_OBJECT_FIRST_NOTIFICATION_TYPE     NSRM_NSCHAIN_SET_OBJECT_ALL          
#define NSRM_NSCHAINSET_OBJECT_LAST_NOTIFICATION_TYPE      NSRM_NSCHAIN_SET_OBJECT_ATTRIBUTE_DELETED          
#define NSRM_L2NW_SERVICE_MAP_FIRST_NOTIFICATION_TYPE        NSRM_L2NW_SERVICE_MAP_ALL      
#define NSRM_L2NW_SERVICE_MAP_LAST_NOTIFICATION_TYPE         NSRM_L2NW_SERVICE_MAP_ATTRIBUTE_DELETED          
#define NSRM_SELECTION_RULE_FIRST_NOTIFICATION_TYPE        NSRM_NSCHAIN_SELECTION_RULE_ALL
#define NSRM_SELECTION_RULE_LAST_NOTIFICATION_TYPE         NSRM_NSCHAIN_SELECTION_RULE_ATTRIBUTE_DELETED

#define NSRM_BYPASS_RULE_FIRST_NOTIFICATION_TYPE        NSRM_NWSERVICE_BYPASS_RULE_ALL
#define NSRM_BYPASS_RULE_LAST_NOTIFICATION_TYPE         NSRM_NWSERVICE_BYPASS_RULE_ATTRIBUTE_DELETED



#if 0
struct nsrm_ip
{
  uint8_t  ip_object_type_e; /* ANY, single, range, subnet */
  uint32_t ipaddr_start;
  uint32_t ipaddr_end; /* numberof mask bits in case of subnet */
};

enum ip_object_type_e
{
  IP_ANY,
  IP_SINGLE,
  IP_RANGE,
  IP_SUBNET
};

struct nsrm_port
{
  uint8_t  port_object_type_e; /* ANY ,single, range */
  uint16_t port_start;
  uint16_t port_end;
};

enum  port_object_type_e
{
  PORT_ANY,
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
  MAC_ANY,
  MAC_SINGLE
};


struct nsrm_ethtype
{
  uint8_t ethtype_type_e; /* ANY, single */
  uint8_t ethtype;
};

enum ethtype_type_e
{
  ETHTYPE_ANY,
  ETHTYPE_SINGLE
};
#endif
/* nwservice_object - Hash Table with name as the key. */

struct nsrm_nwservice_object  
{
  char*   name_p;
  char*   tenant_name_p;   
  uint8_t type_e;      /* Physical, Virtual */

  /* Attributes can be used by CRD consumer to store Open Stack specific
     information, Appliance category, Network functions of each 
     nwservice_object, nwservcie_instance_id, Vendor ID etc... */
   
  uint8_t attach_count;     
  /* used for deletion of neswervice_object. A nwservice_object is  
     allowed to be deleted only after it is detached from all the nschain 
     objects.   */

  uint8_t admin_status_e; /* By default it is enabled*/
  /* Config Apllication  may use the API to change status at any time. */

  uint8_t operational_status_e;
  /* Controlled internally */ 
  
  uint8_t attach_status; /*Indicate if nwservice is attached to particular nschain */
  
  uint8_t bypass_attach_count; /*Indicate if the nwservice exists in any bypass rule*/
  
  /* NSM */
  uint8_t  nwservice_type_e;  /* nsm */
  uint8_t  load_sharing_algorithm_e;
  uint8_t  load_indication_type_e;
  uint64_t high_threshold;
  uint64_t low_threshold;


  uint8_t  heap_b;
  uint32_t magic;
  struct   mchash_table_link nwservice_object_tbl_link;
  uint32_t index;
  uint64_t nwservice_object_handle;
 
  uint32_t  number_of_attributes;
  of_list_t attributes;

};

/* nschain_object */
/* This is an internal structure. A hash table holds the ns_chain objects configured with name as the key */

struct nsrm_nschain_object
{
  char*   name_p;
  char*   tenant_name_p;
  uint8_t nschain_type;
 
  uint8_t no_of_nw_service_objects; /* NSM Added on 17th March 2014 */
  struct mchash_table* nwservice_objects_list;
 /* data structure nschain_nwservice_object */ 
 /* list of nwservice objects each of which contain name and safe reference to the nwservice_object.
    It also includes a list of nwservice_instance variable.List is implemented using hash table with hash buckets = 1 */
  
  uint8_t no_of_rules;   
  struct mchash_table* bypass_rule_list; 
  /* Ordered set of bypass rules. No of buckets = 1. */

  uint8_t  packet_fields_to_hash;

#if 0
  uint8_t load_sharing_algorithm_e; /* We implement ROUND_ROBIN only */
  /* HASH_BASED_ALGORITHM, ROUND_ROBIN, LEAST_CONNECTIONS */

  uint8_t packet_fields_to_hash; /* define bit map for HASH_BASED_ALGORITHM */
 
  uint8_t load_indication_type_e; /* We implement CONNECTION_BASED_ALGORITHM only */
  /* CONNECTION_BASED,TRAFFIC_BASED */
  uint64_t high_threshold; /* We need to report to Application when either of these limits is crossed */
  uint64_t low_threshold;
#endif
  uint64_t no_of_existing_connections; /* Compared against the threshold values every time this variable is incremented or decremented */
                                          
  uint8_t nwservice_instance_count;
  /* This is to be incremented each time a nwservice_object is instantiated or launched for the nschain_object.
     A nwservice_object can be detached from a nschain_object only if all its instances in the nschain object are de-launched. */

  uint8_t nschain_usage_count;
  /* This is to be incremented each time the nschain_object is added to a nschain_set_object */

  uint8_t admin_status_e; /* By default it is enabled*/
  /* Config Appllication  may use the API to change status at any time. */

  uint8_t operational_status_e;
  /* Controlled internally */ 

 
  uint8_t  heap_b;
  uint32_t magic;
  struct   mchash_table_link nschain_object_tbl_link;
  uint32_t index;
  uint64_t nschain_object_handle;

  uint32_t  number_of_attributes;
  of_list_t attributes;

};

struct nsrm_nwservice_bypass_rule
{
  char* name_p;

  /* Match Fields */
  struct nsrm_mac src_mac;
  struct nsrm_mac dst_mac;
  struct nsrm_ethtype eth_type;
  struct nsrm_ip   sip;
  struct nsrm_ip   dip;
  struct nsrm_port sp;
  struct nsrm_port dp;
  uint16_t ip_protocol;
  uint8_t location_e;
  
  /* Action */
  uint8_t no_of_nwservice_objects; 
  /* Network services (objects) that are to be skipped for packets matching with this bypass rule */
  of_list_t skip_nwservice_objects;  /* name and safereference of each nwservice_object to be bypassed */

/*--------------------- */
  /* Network services (objects) to be applied for packets matching with this bypass rule */
  /* This list is deduced from the skip configuration */
  of_list_t apply_nwservice_objects; /* Name and Saferef of each object to be applied */ 
  
  /* nschain to which this bypass rule is added */
  uint64_t nschain_object_saferef;

/*----------------------*/   
  uint8_t admin_status_e; /* By default it is enabled*/
  /* Config Apllication  may use the API to change status at any time. */

  uint8_t operational_status_e;
  /* Controlled internally */ 

  char*    relative_name_p; 
  uint8_t  heap_b;
  uint32_t magic;
  struct   mchash_table_link bypass_rule_tbl_link;
  uint32_t index;
  uint64_t bypass_rule_handle;

  uint32_t  number_of_attributes;
  of_list_t attributes;
};

struct nwservice_object_skip_list
{
   of_list_node_t   next_node;   /* next nwservice in the list */
   struct rcu_head  rcu_head;
   uint8_t          heap_b;
   char*            nwservice_object_name_p;
   uint64_t         nwservice_object_handle;
};


struct nschain_nwservice_object
{
  char*       name_p;
  uint64_t    nwservice_object_saferef;
  uint16_t     sequence_number;
  uint8_t     nwservice_instance_count; /*Temporarily  added by Vinod for compilation purpose*/
  struct mchash_table* nwservice_instance_list; /*Hash buckets = 1 */

  /* runtime information */
  uint64_t   next_nwservice_instance_to_be_used;
  /* We use roundrobin algorithm to select the nwservice_instance for the next connection.*/ 
  /* Only a new connection visits this piece of code. */
  /* A selection rule may contain ranges and wild cards but a connection contains exact values. */
  /* Repository contains connection entries and the nwservice_instance references.
     If no connection entry is present for a packet, it finds a matching selection rule 
     and comes here to select the nwservice_instance to use for the connection.*/   
  /* For each connection, we use this as a running variable to choose the nwservice_instance to use.*/
  /* We don't come here if a connection entry is present in repository for the packet. */  
  uint8_t  heap_b;
  uint32_t magic;
  struct   mchash_table_link nschain_nwservice_object_tbl_link;
  uint32_t index;

  uint64_t nschain_nwservice_handle;

};

/* This structure is part of a list of nwservice_objects in nschain_object each element in the list 
   is used to maintain a list of nwservice_instances. */

struct nwservice_instance
{
  char*    name_p;
  char*    vm_name_p;
  char*    tenant_name_p;
  char*    nschain_object_name_p;
  char*    nwservice_object_name_p;
  char     switch_name[128];
  uint64_t vm_saferef;
  uint16_t vlan_id_in;
  uint16_t vlan_id_out;
  
  char     vm_ns_port_name_p[16];
  uint64_t vm_ns_port_saferef; 
  uint32_t port_id;
  
  /* Line Added to support addition of flows at once NSMSRINI */
  uint64_t system_br_saferef;

  uint8_t  heap_b;
  uint32_t magic;
  struct   mchash_table_link nwservice_instance_tbl_link;
  uint32_t index;
  uint64_t srf_nwservice_instance;

};
/* This structure is used to instantiate nwservice_objects with in a nschain_objet. */ 

/*nschainset_object*/
/* A hash table is used to hold the service chain sets configured with name as the key */

struct nsrm_nschainset_object
{
  char*   tenant_name_p;
  char*   name_p;
  uint8_t nschainset_type;

  uint8_t no_of_rules;
          
  struct mchash_table* nschain_selection_rule_set;
  /* Ordered list.  No of buckets = 1  */

  uint8_t admin_status_e; /* By default it is enabled*/
  /* Config Apllication  may use the API to change status at any time. */

  uint8_t operational_status_e;
  /* Controlled internally */ 
  uint8_t  heap_b;
  uint32_t magic;
  struct   mchash_table_link nschainset_object_tbl_link;
  uint32_t index;
  uint64_t nschainset_object_handle;

  uint32_t  number_of_attributes;
  of_list_t attributes;


};

struct nsrm_nschain_selection_rule
{
  
  char* name_p;

  /* Match Fields */
  struct nsrm_mac src_mac;
  struct nsrm_mac dst_mac;

  struct nsrm_ethtype eth_type;
  struct  nsrm_ip sip;
  struct  nsrm_ip dip;
  struct  nsrm_port sp;
  struct  nsrm_port dp;
  uint16_t ip_protocol;
  uint8_t location_e;

  /* Action */
  char*    nschain_object_name_p;
  char*    relative_name_p;
  uint64_t nschain_object_saferef;
  uint64_t nschainset_object_saferef;

  uint8_t admin_status_e; /* By default it is enabled*/
  /* Config Apllication  may use the API to change status at any time. */

  uint8_t operational_status_e;
  /* Controlled internally */ 
  uint8_t  heap_b;
  uint32_t magic;
  struct   mchash_table_link selection_rule_tbl_link;
  uint32_t index;
  uint64_t selection_rule_handle;
 
  uint32_t  number_of_attributes;
  of_list_t attributes;


};

struct packet_into
{
   uint32_t   src_ip;
   uint32_t   dst_ip;
   uint32_t   src_mac_high;
   uint32_t   src_mac_low;
   uint32_t   dst_mac_high;
   uint32_t   dst_mac_low;
   uint16_t   src_port;
   uint16_t   dst_port;
   uint8_t    ethtype_e;
};



/*l2nw_service_map*/
/* A hash table is used to hold the l2 service map records configured with map_name as the key */

struct nsrm_l2nw_service_map
{
  char* tenant_name_p;
  char* map_name_p;
  
  char* vn_name_p;
  char* nschainset_object_name_p;
  
  uint64_t vn_saferef;
  uint64_t nschainset_object_saferef; 

  uint8_t admin_status_e; /* By default it is enabled*/
  /* Config Apllication  may use the API to change status at any time. */

  uint8_t operational_status_e;
  /* Controlled internally */ 
  uint8_t  heap_b;
  uint32_t magic;
  struct   mchash_table_link l2nw_service_map_tbl_link;
  uint32_t index;
  uint64_t l2nw_service_map_handle;

  uint32_t  number_of_attributes;
  of_list_t attributes;
};

/*l3nw_service_map*/
/* A hash table is used to hold the l3 service map records configured with map_name as the key */

struct nsrm_l3nw_service_map
{
  char* tenant_name_p;
  char* map_name_p;

  uint8_t no_of_networks;
  struct mchash_table* l3_virtual_network_list;

  char* nschainset_object_name_p;
  uint64_t nschainset_object_saferef;

  uint8_t admin_status_e; /* By default it is enabled*/
  /* Config Apllication  may use the API to change status at any time. */

  uint8_t operational_status_e;
  /* Controlled internally */ 
};

struct nsrm_l3_vn
{
  char*    vn_name_p;
  uint64_t vn_saferef;
};

/*Internal APIs*/

/* Add_attribute and Del_attribute API internally create/delete view entries
   Provides a view is created for the database. */

/* Internal API */ 
int32_t nsrm_create_nwservice_object_view_entry (
       uint64_t nwservice_object_handle,
       char* view_name_p,
       char* view_value_p);
/* Called from add attribute */
int32_t nsrm_remove_nwservice_object_view_entry(char* view_name,char* view_value);
/* Called from delete attribute */

int32_t nsrm_get_hashval_byname(char* name_p,uint32_t no_of_buckets);

int32_t nsrm_get_nschain_object_handle(char *nschain_object_name_p, uint64_t *nschain_object_handle_p);
int32_t nsrm_get_nschain_byhandle(uint64_t nschain_handle,
                                  struct nsrm_nschain_object **nschain_object_p_p);


int32_t nsrm_get_nschainset_object_handle(char *nschainset_object_name_p, uint64_t *nschainset_object_handle_p);
int32_t nsrm_get_nschainset_byhandle(uint64_t nschainset_handle,
                                     struct nsrm_nschainset_object **nschainset_object_p_p);

int32_t nsrm_get_bypass_rule_byhandle(struct nsrm_nschain_object *nschain_object_p ,
                                      uint64_t bypass_rule_handle ,
                                      struct nsrm_nwservice_bypass_rule **bypass_rule_p_p);



int32_t nsrm_get_l2nw_service_map_handle(char *l2nw_service_map_name_p, uint64_t *l2nw_service_map_handle_p);
int32_t nsrm_get_l2_smap_record_byhandle(uint64_t l2map_handle,
                                         struct nsrm_l2nw_service_map **l2nw_service_map_p_p);

void nsrm_attributes_free_attribute_entry_rcu(struct rcu_head *attribute_entry_p);

int32_t nsrm_get_selection_rule_handle(char *rule_name_p, struct nsrm_nschainset_object *nschainset_object_p,uint64_t *selection_rule_handle_p);

int32_t nsrm_get_selection_rule_byhandle(struct nsrm_nschainset_object *nschainset_object_p ,
                                         uint64_t selection_rule_handle ,
                                         struct nsrm_nschain_selection_rule  **selection_rule_p_p);
int32_t nsrm_get_nwservice_object_handle(char *nwservice_object_name_p, uint64_t *nwservice_object_handle_p);

int32_t nsrm_get_nwservice_byhandle(uint64_t nwservice_handle,struct nsrm_nwservice_object
                                                              **nwservice_object_p_p);
int32_t nsrm_add_attribute(of_list_t *attributes_p, struct nsrm_attribute_name_value_pair* attribute_info_p);

int32_t nsrm_get_first_attribute( of_list_t *attributes_p,
                                    struct nsrm_attribute_name_value_output_info  *attribute_output_p);

int32_t nsrm_get_next_attribute(of_list_t *attributes,char *current_attribute_name,
                                struct nsrm_attribute_name_value_output_info *attribute_output);

int32_t nsrm_get_exact_attribute(of_list_t *attributes_p,
      char  *attribute_name_string,struct nsrm_attribute_name_value_output_info *attribute_output);

void nsrm_free_view_node_rcu(struct rcu_head *view_node_p);
int32_t nsrm_view_hash_table_init(struct mchash_table **view_node_table);

struct database_view_node *nsrm_alloc_view_entry_and_set_values(uint64_t saferef, char *node_name, char *view_value);

void    nsrm_get_view_node_mempoolentries(uint32_t *view_node_entries_max,uint32_t *view_node_static_entries);


int32_t nsrm_view_hash_table_deinit(struct mchash_table* view_node_table);

int32_t nsrm_get_attached_nwservice_byhandle(uint64_t nwservice_handle,
                                             uint64_t nschain_handle,
                                             struct nschain_nwservice_object **nschain_nwservice_object_p_p);
int32_t nsrm_create_bypass_rule_view_entry(uint64_t bypass_handle , char* view_name ,char* view_value);
int32_t nsrm_remove_bypass_rule_view_entry(char* view_name,char* view_value);
int32_t nsrm_create_l2nw_map_view_entry(uint64_t l2nw_map_handle , char* view_name ,char* view_value);

int32_t nsrm_remove_l2nw_map_view_entry(char* view_name,char* view_value);
int32_t nsrm_create_selection_rule_view_entry(uint64_t selection_handle , char* view_name ,char* view_value);

int32_t nsrm_remove_selection_rule_view_entry(char* view_name,char* view_value);
int32_t nsrm_create_nschainset_object_view_entry(uint64_t nschainset_handle , char* view_name ,char* view_value);
int32_t nsrm_remove_nschainset_object_view_entry(char* view_name,char* view_value);
int32_t nsrm_create_nschain_object_view_entry(uint64_t nschainset_handle , char* view_name ,char* view_value);
int32_t nsrm_remove_nschain_object_view_entry(char* view_name,char* view_value);


int32_t nsrm_get_attached_nwservice_handle(uint64_t nschain_handle,
                                             char* nschain_nwservice_name,
                                             uint64_t* nschain_nwservice_handle);

int32_t nsrm_get_nwservice_instance_byhandle(uint64_t nschain_handle,
                                             uint64_t nschain_nwservice_object_handle,
                                             uint64_t nwservice_instance_handle,
                                             struct   nwservice_instance **nwservice_instance_p_p);

