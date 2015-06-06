

#define OF_GET_VIEW_DATABASE_INDEX(a,view_index) \
{\
	int32_t ii;\
	for(ii=0; ii < DPRM_MAX_VIEW_DATABASE; ii++) \
	{ \
		if(!strcmp(of_view_database_name[ii], a)) \
			break;\
	} \
	view_index=ii;\
       OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "view_index %d",view_index);\ 
}


#define OF_MAX_VIEW_NODE_ENTRIES              1024
#define OF_MAX_VIEW_STATIC_NODE_ENTRIES         256 





#define OF_VIEW_ENTRY_NODE_HASHTBLE_LINK_OFFSET 0

//#define OF_VIEW_ENTRY_NODE_HASHTBLE_LINK_OFFSET offsetof(struct database_view_node,view_tbl_lnk);

#define  OF_VIEW_MODU32(a,b) (a%b)
#define  OF_VIEW_DIVU32(a,b) (a/b)



/*
      This structure represents one view of a database. Namespace view of a logical switch (dp) is one
      such data base view. A database view is normally specified using the name of an attribute added to
     the database nodes.

     For each database view, a separate hash table is maintained to hold the database nodes
     (struct database_view_node) which are arranged on the basis of the view.

      The uniqueness_flag when set to 1 indicates that some nodes in the database may contain attributes
     whose attribute values are identical.

*/
struct database_view
{ 
	char view[64];                                  /* attribute_name */
	struct mchash_table*  db_viewtbl_p;  /* Hash table holding the database_view_nodes. */ 
	uint8_t  uniqueness_flag;        /* uniqueness_flag =1 node_name is not used in hashing. */
	uint8_t  valid_b;        /* validity */
};
/*
     This structure represents one node in the hash table of a given database_view. 
     It contains the safe reference to the actual node in the database. 
*/
struct database_view_node
{
	struct       mchash_table_link  view_tbl_lnk;  /* to link this view node to the view Hash table */ 
	char*      node_name;                                 /* name of the node */
	char*      view_value;                                 /* attribute value */
	uint64_t  node_saferef;                              /* safe reference to the actual database node */
	int32_t index;
	int32_t magic;
};


/* VIEW Init */
void of_view_init();
int32_t of_view_node_init();
int32_t of_view_hash_table_init(struct mchash_table **view_node_table);
void of_free_view_node_rcu(struct rcu_head *database_view_node_p);
void of_get_view_node_mempoolentries(uint32_t* view_node_entries_max,uint32_t* view_node_static_entries);




uint32_t dprm_get_hashval_bynames(char* name1_p,char *name2_p,uint32_t no_of_buckets);

