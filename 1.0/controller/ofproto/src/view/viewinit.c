

#include "controller.h"
#include "dprm.h"
#include "cntlr_epoll.h"
#include "cntrl_queue.h"
#include "of_utilities.h"
#include "of_msgapi.h"
#include "of_multipart.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_event.h"
#include "of_view.h"
#include "viewldef.h"





void *of_view_node_mempool_g=NULL;
struct database_view dprm_db_view[DPRM_MAX_VIEW_DATABASE];

int32_t of_no_of_view_node_buckets_g;


void of_view_init()
{
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  of_view_node_init();
}

int32_t of_view_node_init()
{
  int32_t  ret_value;
  uint32_t view_node_entries_max,view_node_static_entries;
  struct mempool_params mempool_info={};

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  of_get_view_node_mempoolentries(&view_node_entries_max,&view_node_static_entries);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
  mempool_info.block_size = sizeof(struct database_view_node);
  mempool_info.max_blocks = view_node_entries_max;
  mempool_info.static_blocks = view_node_static_entries;
  mempool_info.threshold_min = view_node_static_entries/10;
  mempool_info.threshold_max = view_node_static_entries/3;
  mempool_info.replenish_count = view_node_static_entries/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("of_view_node_pool", &mempool_info,
		  &of_view_node_mempool_g
		  );

  if(ret_value != MEMPOOL_SUCCESS)
    return OF_FAILURE;

  of_no_of_view_node_buckets_g = (view_node_entries_max / 5*DPRM_MAX_VIEW_DATABASE) + 1;
  return OF_SUCCESS;
}

int32_t of_view_hash_table_init(struct mchash_table **view_node_table)
{
  int32_t  ret_value;
  uint32_t view_node_entries_max,view_node_static_entries;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  of_get_view_node_mempoolentries(&view_node_entries_max,&view_node_static_entries);
  ret_value = mchash_table_create(((view_node_entries_max / 5 *DPRM_MAX_VIEW_DATABASE)+1),
      view_node_entries_max/DPRM_MAX_VIEW_DATABASE,
      of_free_view_node_rcu,
      view_node_table);
  if(ret_value != MCHASHTBL_SUCCESS)
    return OF_FAILURE;

    return OF_SUCCESS;

}

void of_free_view_node_rcu(struct rcu_head *view_node_p)
{
	struct database_view_node *database_view_node_p=(struct database_view_node *)view_node_p;
	if(database_view_node_p)
	{
		mempool_release_mem_block(of_view_node_mempool_g,(uchar8_t*)database_view_node_p,FALSE);
		if( database_view_node_p->node_name)
		{
			free(database_view_node_p->node_name);
		}
		if( database_view_node_p->view_value)
		{
			free(database_view_node_p->view_value);
		}
	}
	else
	{
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," NULL passed for deletion.");
	}
}

void of_get_view_node_mempoolentries(uint32_t* view_node_entries_max,uint32_t* view_node_static_entries)
{
  *view_node_entries_max    = OF_MAX_VIEW_NODE_ENTRIES;
  *view_node_static_entries = OF_MAX_VIEW_STATIC_NODE_ENTRIES;
}
