#define OF_NEM_MAX_NAMESPACE_ENTRIES 1024
#define OF_NEM_MAX_NAMESPACE_STATIC_ENTRIES 256
#define NEM_SUCCESS 0

/** \ingroup Error_Management
 * NEM_FAILURE 
 * - NEM API Functions return this value on general failure in execution
 */
#define NEM_FAILURE -1

/** \ingroup Error_Management
 * NEM_ERROR_NO_MEM 
 * - No memory available to create the database record.
 */
#define NEM_ERROR_NO_MEM -2

/** \ingroup Error_Management
 * NEM_ERROR_NAME_LENGTH_NOT_ENOUGH 
 * - Name of the resource must be atleast 8 characters.
 */
#define NEM_ERROR_NAME_LENGTH_NOT_ENOUGH -3

/** \ingroup Error_Management
 *  NEM_ERROR_NO_MORE_ENTRIES 
 *  - No more resource entries in the database.
 */

#define NEM_ERROR_NO_MORE_ENTRIES -4


#define NEM_ERROR_INVALID_NAME -5
#define NEM_ERROR_INVALID_HANDLE -6
#define NEM_ERROR_DUPLICATE_RESOURCE -7
#define NEM_ERROR_INVALID_NOTIFICATION_TYPE -8
#define NEM_ERROR_NULL_CALLBACK_NOTIFICATION_HOOK -9
#define NEM_ERROR_INVALID_NSID -10


#define NEM_GENERATE_NS_ID(nsid) \
{\
   nem_ns_id_g++;\
   if ( nem_ns_id_g == NEM_MAX_NS_ID )\
   nem_ns_id_g=0;\
   nsid=nem_ns_id_g;\
}

  
#define NEM_GENERATE_PETH_ID(pethid) \
{\
nem_peth_id_g++;\
if ( nem_peth_id_g == NEM_MAX_NS_ID )\
nem_peth_id_g=0;\
pethid=nem_peth_id_g;\
}  


#define NEM_PRINT_NS_AND_NSID_MAP_ENTRY(map_entry_p) \
{\
   OF_LOG_MSG(OF_NEM_LOG, OF_LOG_DEBUG, "nem db entry ns-name=%s ns-id=%d",map_entry_p->ns_info.name,map_entry_p->ns_info.id); \
}

#define NEM_PRINT_NS_AND_NSID_MAP_INFO(ns_info_p) \
{\
   OF_LOG_MSG(OF_NEM_LOG, OF_LOG_DEBUG, "nem info ns-name=%s ns-id=%d",ns_info_p->name,ns_info_p->id); \
}

#define NEM_PRINT_DPID_AND_NSID_MAP_ENTRY(map_entry_p) \
{\
   OF_LOG_MSG(OF_NEM_LOG, OF_LOG_DEBUG, "nem db entry dpid=%llx dp_ns_id=%d ns-id=%d",map_entry_p->dp_ns_info.dp_id,map_entry_p->dp_ns_info.dp_ns_id, map_entry_p->dp_ns_info.ns_id); \
}

#define NEM_PRINT_DPID_AND_NSID_MAP_INFO(dp_ns_info_p) \
{\
   OF_LOG_MSG(OF_NEM_LOG, OF_LOG_DEBUG, "nem info dpid=%llx dp_ns_id=%d ns-id=%d",dp_ns_info_p->dp_id,dp_ns_info_p->dp_ns_id, dp_ns_info_p->ns_id); \
}

#define NEM_PRINT_DPID_AND_PETH_MAP_ENTRY(map_entry_p) \
{\
   OF_LOG_MSG(OF_NEM_LOG, OF_LOG_DEBUG, "nem db entry dpid=%llx ns-id=%d port-id=%d peth-name=%s",map_entry_p->dp_peth_info.dp_id,map_entry_p->dp_peth_info.ns_id, map_entry_p->dp_peth_info.port_id,map_entry_p->dp_peth_info.peth_name); \
}

#define NEM_PRINT_DPID_AND_PETH_MAP_INFO(dp_peth_info_p) \
{\
   OF_LOG_MSG(OF_NEM_LOG, OF_LOG_DEBUG, "nem info dpid=%llx  ns-id=%d port-id=%d peth-name=%s",dp_peth_info_p->dp_id, dp_peth_info_p->ns_id,dp_peth_info_p->port_id, dp_peth_info_p->peth_name); \
}

#define NEM_PRINT_DPID_AND_DPNSID_MAP_ENTRY(map_entry_p) \
{\
   OF_LOG_MSG(OF_NEM_LOG, OF_LOG_DEBUG, "nem db entry dpid=%llx dp_ns_id=%d namespace=%s",map_entry_p->dp_ns_info.dp_id,map_entry_p->dp_ns_info.dp_ns_id, map_entry_p->dp_ns_info.namespace_name); \
}

#define NEM_PRINT_DPID_AND_DPNSID_MAP_INFO(dp_ns_info_p) \
{\
   OF_LOG_MSG(OF_NEM_LOG, OF_LOG_DEBUG, "nem info dpid=%llx dp_ns_id=%d namespace=%s",dp_ns_info_p->dp_id,dp_ns_info_p->dp_ns_id, dp_ns_info_p->namespace_name); \
}

/******************************************************************************
 * * * * * * * * * * * * Notifications * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
/**List of Applications registered to receive DPRM resource (sw,domain,datapath,table,port) related notifications
 *like addition,deletion,modification etc...Applications may regiser for a specfic notification or all the notifications
 *related to a specific resource(sw,domain etc..).Priority is not maintained.
 */

struct nem_notification_app_hook_info
{
   /** to traverse next application registered for a dprm resource notifications */
   of_list_node_t  next_app;
   /** for RCU operation*/
   struct rcu_head  rcu_head;
   /** Application callback function to call*/
   void  *call_back;             
   /** Application specific private info*/
   void  *cbk_arg1;
   /** Application specific private info*/
   void  *cbk_arg2;
   /** Boolean flag indicate whether this memory is allocated from heap or not*/
   uint8_t heap_b;
};

#define NEM_NOTIFICATIONS_APP_HOOK_RCU_HEAD_OFFSET offsetof(struct nem_notification_app_hook_info,rcu_head)
#define NEM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET offsetof(struct nem_notification_app_hook_info,next_app)

/*
   This structure will be used as member for ns_and_nsid_mapping_repository hash table.
 */

struct nem_ns_and_nsid_map_entry
{
   struct mchash_table_link hash_tbl_link;   /* It is used by mchash table infrastructure to attach a this node to a ns_nsid_maping hash table  repository*/
   uint32_t index;   /* index of the safe reference to this node entry in mchash table */
   uint32_t magic;   /* index of the safe reference to this node entry in mchash table */
   struct of_nem_ns_info ns_info;   /* pointer to the namespace entry struct which holds the o mapping values of namespace name and id */
   uint8_t heap_b;
};

#define NS_NSID_MAPNODE_HASH_TBLLNK_OFFSET offsetof(struct nem_ns_and_nsid_map_entry,hash_tbl_link);

/*
   This structure will be used as member of veth_to_namespace mapping repository.  Veth interface Name used for generate a hash key which will be in turn used as hash table index.*/
struct nem_dpid_and_peth_map_entry
{
   struct mchash_table_link hash_tbl_link;
   /* It is used by mchash table infrastructure to attach a this node to a nsid_2_peth_maping hash table repository*/
   uint32_t index; /* index of the safe reference to this node entry in mchash table */
   uint32_t magic;   /* index of the safe reference to this node entry in mchash table */
   struct of_nem_dp_peth_info dp_peth_info;
   uint8_t heap_b;
};

#define DPID_PETH_MAPNODE_HASH_TBLLNK_OFFSET offsetof(struct nem_dpid_and_peth_map_entry,hash_tbl_link);

/*
   This structure will be used as member of dpid_to_nsid mapping repository. Here dpid used for generate a hash key used as hash table index.
 */
struct of_nem_dpid_dpns_info
{
   uint64_t dp_id;
   uint16_t dp_ns_id;
   char namespace_name[NEM_MAX_NS_NAME_LEN]; /* name of the name space   */
   int8_t valid_b;
};

struct nem_dpid_and_nsid_map_entry
{
   struct mchash_table_link hash_tbl_link;
   /* It is used by mchash table infrastructure to attach a this node to a nsid_2_peth_maping hash table repository*/
   uint32_t index; /* index of the safe reference to this node entry in mchash table */
   uint32_t magic;   /* index of the safe reference to this node entry in mchash table */
   struct of_nem_dp_ns_info dp_ns_info;  /* identifier of datapath and  generated local Namespace Id and  Name space Id */
   uint8_t heap_b;
};

struct nem_dpid_ns_info
{
   uint64_t dp_id;
   uint16_t dp_ns_id;
   int8_t valid_b;
};

struct nem_dpid_and_dpnsid_map_entry
{
   struct mchash_table_link hash_tbl_link;
   /* It is used by mchash table infrastructure to attach a this node to a nsid_2_peth_maping hash table repository*/
   uint32_t index; /* index of the safe reference to this node entry in mchash table */
   uint32_t magic;   /* index of the safe reference to this node entry in mchash table */
   struct of_nem_dpid_dpns_info dp_ns_info;  /* identifier of datapath and  generated local Namespace Id and  Name space Id */
   uint8_t heap_b;
};


#define DPID_NSID_MAPNODE_HASH_TBLLNK_OFFSET offsetof(struct nem_dpid_and_nsid_map_entry,hash_tbl_link);

#define NEM_MAX_NS_ID 50000

#define OF_NEM_MAX_NS_TO_NSID_MAP_ENTRIES 1024

#define OF_NEM_MAX_NS_TO_NSID_MAP_BUCKETS          (OF_NEM_MAX_NAMESPACE_ENTRIES /5 ) +1

#define OF_NEM_MAX_NSID_TO_DPID_MAP_BUCKETS          (OF_NEM_MAX_NSID_TO_DPID_MAP_ENTRIES /5 ) +1

#define OF_NEM_MAX_NSID_MAP_BUCKETS_IN_DPIDTBLE (OF_NEM_MAX_NSID_MAP_ENTRIES_IN_DPIDTBL)/5 +1

#define OF_NEM_MAX_VETH_MAP_BUCKETS_IN_NSIDTBLE (OF_NEM_MAX_VETH_MAP_ENTRIES_IN_NSIDTBL)/5 +1

/******************************************************************************
 * * * * * * * * * * * * * * NEM INIT * * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t of_nem_init();

/******************************************************************************
 * * * * * * * * * * * * * * NEM Events * * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t nem_event_init();
int32_t nem_event_mempool_init();
int32_t nem_event_register();
void nem_get_notification_mempoolentries(uint32_t *notification_entries_max,uint32_t *notification_static_entries);

/******************************************************************************
 * * * * * * * * * * * * * * NEM Database * * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t nem_db_init();
int32_t nem_ns_and_nsid_mapping_handler(char *namespace_p, uint32_t ns_id);
int32_t nem_dpid_dpnsid_mapping_handler(uint64_t dp_id, char *namespace_p,  uint16_t *dp_ns_id_p);
int32_t nem_dpid_nsid_mapping_handler(uint64_t dp_id, uint16_t dp_ns_id, uint32_t ns_id);
int32_t nem_dpid_peth_mapping_handler(uint64_t dp_id, uint32_t port_id, uint32_t ns_id, char *peth_name_p, uint32_t peth_id);
int32_t nem_ns_and_nsid_unmapping_handler(char *namespace_p, uint32_t ns_id);
int32_t nem_dpid_dpnsid_unmapping_handler(uint64_t dp_id, char *namespace_p, uint16_t *dp_ns_id_p);
int32_t nem_dpid_nsid_unmapping_handler(uint64_t dp_id, uint32_t ns_id, uint16_t dp_ns_id);
int32_t nem_dpid_peth_unmapping_handler(uint64_t dp_id, uint32_t port_id, char *peth_name_p);

/******************************************************************************
 * * * * * * * * * * * * Namespace to NSID * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t nem_ns_2_nsid_db_init();
int32_t nem_ns_2_nsid_db_mempool_init();
int32_t nem_ns_2_nsid_db_mchash_table_init();
int32_t nem_validate_namespace_and_generate_nsid(char *namespace_p, uint32_t *ns_id_p);


int32_t nem_map_ns_to_nsid(struct of_nem_ns_info *ns_info_p,uint64_t*  output_handle_p);
int32_t nem_unmap_ns_to_nsid(uint64_t handle);
int32_t nem_update_ns_to_nsid_map_entry(uint64_t handle,struct of_nem_ns_info* ns_info_p);

/******************************************************************************
 * * * * * * * * * * * * NSID to Namespace * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t nem_nsid_2_ns_db_init();
int32_t nem_nsid_2_ns_db_mempool_init();
int32_t nem_nsid_2_ns_db_mchash_table_init();

int32_t nem_map_nsid_to_ns(struct of_nem_ns_info *ns_info_p,uint64_t*  output_handle_p);
int32_t nem_unmap_nsid_to_ns(uint64_t handle);
int32_t nem_update_nsid_to_ns_map_entry(uint64_t handle,struct of_nem_ns_info* ns_info_p);

/******************************************************************************
 * * * * * * * * * * * * NSID and Namespace * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
void nem_ns_and_nsid_db_get_map_entry_mempoolentries(uint32_t* db_entries_max,uint32_t* db_static_entries);

/******************************************************************************
 * * * * * * * * * * * * DPID and  DPNSID * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t nem_dpid_and_dpnsid_db_init();
int32_t nem_dpid_and_dpnsid_db_mempool_init();
int32_t nem_dpid_and_dpnsid_db_mchash_table_init();

int32_t nem_map_dpid_to_dpnsid(struct of_nem_dpid_dpns_info *dp_ns_info_p,uint64_t*  output_handle_p);
int32_t nem_unmap_dpid_to_dpnsid(uint64_t handle);
int32_t nem_update_dpid_to_dpnsid_map_entry(uint64_t handle,struct of_nem_dpid_dpns_info* dp_ns_info_p);
int32_t nem_dpid_and_dpnsid_db_get_first_map_entry(struct of_nem_dpid_dpns_info *dp_ns_info_p,
      uint64_t  *map_entry_handle_p);
int32_t nem_dpid_and_dpnsid_db_get_next_map_entry(struct of_nem_dpid_dpns_info *dp_ns_info_p,
      uint64_t *map_entry_handle_p);
int32_t  nem_dpid_and_dpnsid_db_get_exact_map_entry(uint64_t handle,
      struct of_nem_dpid_dpns_info* dp_ns_info_p);

int32_t nem_dpid_and_dpnsid_db_get_map_entry_handle(uint64_t dp_id, char *namespace_p, uint64_t* map_entry_handle_p);
int32_t nem_validate_namespace_and_generate_dpnsid_for_dpid(uint64_t dpid, char *namespace_p, uint16_t *dp_ns_id_p);


/******************************************************************************
 * * * * * * * * * * * * DPID to NSID * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t nem_dpid_2_nsid_db_init();
int32_t nem_dpid_2_nsid_db_mempool_init();
int32_t nem_dpid_2_nsid_db_mchash_table_init();

int32_t nem_map_dpid_to_nsid(struct of_nem_dp_ns_info *dp_ns_info_p,uint64_t*  output_handle_p);
int32_t nem_unmap_dpid_to_nsid(uint64_t handle);
int32_t nem_update_dpid_to_nsid_map_entry(uint64_t handle,struct of_nem_dp_ns_info* dp_ns_info_p);

/******************************************************************************
 * * * * * * * * * * * * NSID to DPID * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t nem_nsid_2_dpid_db_init();
int32_t nem_nsid_2_dpid_db_mempool_init();
int32_t nem_nsid_2_dpid_db_mchash_table_init();

int32_t nem_map_nsid_to_dpid(struct of_nem_dp_ns_info *dp_ns_info_p,uint64_t*  output_handle_p);
int32_t nem_unmap_nsid_to_dpid(uint64_t handle);
int32_t nem_update_nsid_to_dpid_map_entry(uint64_t handle,struct of_nem_dp_ns_info* dp_ns_info_p);

/******************************************************************************
 * * * * * * * * * * * * DPID and NsID * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
void nem_dpid_and_nsid_db_get_map_entry_mempoolentries(uint32_t* db_entries_max,uint32_t* db_static_entries);

/******************************************************************************
 * * * * * * * * * * * * DPID to PETH * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t nem_dpid_2_peth_db_init();
int32_t nem_dpid_2_peth_db_mempool_init();
int32_t nem_dpid_2_peth_db_mchash_table_init();

int32_t nem_map_dpid_to_peth(struct of_nem_dp_peth_info *dp_peth_info_p,uint64_t*  output_handle_p);
int32_t nem_unmap_dpid_to_peth(uint64_t handle);
int32_t nem_update_dpid_to_peth_map_entry(uint64_t handle,struct of_nem_dp_peth_info* dp_peth_info_p);

/******************************************************************************
 * * * * * * * * * * * * PETH to DPID * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t nem_peth_2_dpid_db_init();
int32_t nem_peth_2_dpid_db_mempool_init();
int32_t nem_peth_2_dpid_db_mchash_table_init();

int32_t nem_map_peth_to_dpid(struct of_nem_dp_peth_info *dp_peth_info_p,uint64_t*  output_handle_p);
int32_t nem_unmap_peth_to_dpid(uint64_t handle);
int32_t nem_update_peth_to_dpid_map_entry(uint64_t handle,struct of_nem_dp_peth_info* dp_peth_info_p);

/******************************************************************************
 * * * * * * * * * * * * PETH and DPID * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/

int32_t nem_validate_dpid_portid_and_create_peth(uint64_t dp_id, uint32_t port_id, char *ns_name, uint32_t ns_id, char *peth_name_p, uint32_t *peth_id);
void nem_dpid_and_peth_db_get_map_entry_mempoolentries(uint32_t* db_entries_max,uint32_t* db_static_entries);
/******************************************************************************
 * * * * * * * * * * * * NEM UTILITIES * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
uint32_t nem_get_hashval_by_nsid(uint32_t nsid,uint32_t no_of_buckets);
uint32_t nem_get_hashval_by_dpid_and_nsid(uint64_t dpid, uint32_t nsid,uint32_t no_of_buckets);
uint32_t nem_get_hashval_by_nsid_and_peth(uint32_t nsid, char *name_p,uint32_t no_of_buckets);
uint32_t nem_get_hashval_by_dpid_and_nsname(uint64_t dpid, char *name_p,uint32_t no_of_buckets);
uint32_t nem_get_hashval_by_dpid_and_portname(uint64_t dpid, char *name_p,uint32_t no_of_buckets);
uint32_t nem_get_hashval_by_dpid_and_portid(uint64_t dpid, uint32_t port_id,uint32_t no_of_buckets);
/******************************************************************************
 * * * * * * * * * * * * DPID and DPNSID LIST * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
void nem_dpid_and_dpnsid_list_init();
int32_t nem_generate_dpnsid_for_dpid(uint64_t dpid, uint16_t *dp_ns_id_p);
int32_t nem_get_max_dpnsid_for_dpid(uint64_t dp_id, uint16_t *dp_ns_id_p);
int32_t nem_delete_dpid_from_dpid_and_dpnsid_list(uint64_t dpid);
/******************************************************************************
 * * * * * * * * * * * * NEM NAMESPACE UTILITIES * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t  nem_root_ns_init();
int32_t nem_set_cur_task_to_name_space (char *ns_name, int32_t *ns_fd_p);
int32_t nem_set_cur_task_to_root_name_space (int32_t root_ns_fd, int32_t ns_fd);
