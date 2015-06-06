#include "crm_system_dependencies.h"

extern void    *crm_view_node_mempool_g;
void            crm_free_view_node_rcu(struct rcu_head *view_node_p);

/************************************************************************************************/
int32_t crm_view_hash_table_init(struct mchash_table **view_node_table)
{
  int32_t  ret_value;
  uint32_t view_node_entries_max,view_node_static_entries;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered");
  crm_get_view_node_mempoolentries(&view_node_entries_max,&view_node_static_entries);
  ret_value = mchash_table_create(((view_node_entries_max / 5 *CRM_MAX_VIEW_DATABASE)+1),
                                    view_node_entries_max/CRM_MAX_VIEW_DATABASE,
                                    crm_free_view_node_rcu,
                                    view_node_table);
  if(ret_value != MCHASHTBL_SUCCESS)
    return CRM_FAILURE;

    return CRM_SUCCESS;

}
/***********************************************************************************************/
struct database_view_node *crm_alloc_view_entry_and_set_values(uint64_t saferef, char *node_name, char *view_value)
{

  struct database_view_node *view_node_entry_p;
  int32_t ret_value;
  uint8_t heap_b;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered");
  ret_value = mempool_get_mem_block(crm_view_node_mempool_g,(uchar8_t**)&view_node_entry_p,&heap_b);
  if(ret_value != MEMPOOL_SUCCESS)
    return NULL;

  view_node_entry_p->node_name=(char *)calloc(1,128);
  if (view_node_entry_p->node_name == NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,"memory allocation failed");
    mempool_release_mem_block(crm_view_node_mempool_g,(uchar8_t*)view_node_entry_p,FALSE);
    return NULL;
  }

  view_node_entry_p->view_value=(char *)calloc(1,128);
  if (view_node_entry_p->view_value == NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,"memory allocation failed");
    free(view_node_entry_p->node_name);
    mempool_release_mem_block(crm_view_node_mempool_g,(uchar8_t*)view_node_entry_p,FALSE);
    return NULL;
  }

  strcpy(view_node_entry_p->view_value,view_value);
  strcpy(view_node_entry_p->node_name,node_name);
  view_node_entry_p->node_saferef=saferef;
  return view_node_entry_p;
}

int32_t crm_view_hash_table_deinit(struct mchash_table* view_node_table)
{
  if(view_node_table == NULL)
  {
     OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,"Table is NULL");
     return CRM_FAILURE;
  }
  mchash_table_delete(view_node_table);
  return CRM_SUCCESS;
}
