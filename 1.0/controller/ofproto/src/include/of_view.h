
/*
     This structure represents database_view information for external modules.
*/
#define OF_VIEW_NSNAME_DPID "namespace_dpid"
#define OF_VIEW_RESERVED "reserved"
#define OF_MAX_NODE_NAME_LEN 64
#define OF_MAX_VIEW_VALUE_LEN 64

#define DPRM_NAMESPACE_TO_DPID_VIEW_DATABASE 0
#define DPRM_MAX_VIEW_DATABASE 10
struct database_view_node_info
{
	char      node_name[OF_MAX_NODE_NAME_LEN];                                 /* name of the node */
	char      view_value[OF_MAX_VIEW_VALUE_LEN]; 
};



/* VIEW APIs */
uint8_t of_view_get_uniqueness(char *view_name);
int32_t of_get_view_index(char *view_name);
int32_t of_get_view_db_name(int32_t index, char *view_name);
int8_t of_view_db_valid(int32_t view_index);
int32_t of_create_view(char *view_name);
struct database_view_node *of_alloc_view_entry_and_set_values(int64_t saferef, char *node_name, char *view_value);
int32_t  of_create_view_entry(int32_t view_index, char *node_name, char *view_value, uint64_t node_saferef);
int32_t  of_remove_view_entry(int32_t view_index, char *node_name, char *view_value);
int32_t  of_view_get_first_view_entry( int32_t view_index, struct database_view_node_info *db_view_node_info, uint64_t  *view_handle_p);
int32_t  of_view_get_next_view_entry( int32_t view_index, struct database_view_node_info *db_view_node_info, uint64_t  *view_handle_p);
int32_t  of_view_get_first_view_entry_using_key(int32_t view_index, char *view_value, struct database_view_node_info *db_view_node_info, uint64_t  *view_handle_p);
int32_t  of_view_get_next_view_entry_using_key (int32_t view_index, char *view_value, struct database_view_node_info *db_view_node_info, uint64_t  *view_handle_p);
int32_t  of_view_get_view_handle_by_name(int32_t view_index, struct database_view_node_info *db_view_node_info,  uint64_t  *view_handle_p);

/* VIEW Instances Namespace to DPID */
int32_t  of_create_namespace_dpid_view_entry(uint64_t datapath_handle, uint64_t dpid, char *ns_name_p);
int32_t  of_remove_namespace_dpid_view_entry( uint64_t datapath_handle, uint64_t dpid, char *ns_name_p );
int32_t  of_view_get_first_namespace_dpid_view( struct database_view_node_info *db_view_node_info, uint64_t  *view_handle_p);
int32_t  of_view_get_next_namespace_dpid_view( struct database_view_node_info *db_view_node_info, uint64_t  *view_handle_p);
int32_t  of_view_get_namespace_dpid_view_handle_by_name( struct database_view_node_info *db_view_node_info,   uint64_t  *view_handle_p);
int32_t  of_create_vn_view_entry(uint64_t vn_handle, char *view_name_p, char *view_value_p);
int32_t  of_remove_vn_view_entry( uint64_t vn_handle, char *view_name_p, char *view_value_p );
int32_t  of_view_get_crm_view_handle_by_name( struct database_view_node_info *db_view_node_info, char *view_name_p,  uint64_t  *view_handle_p);





