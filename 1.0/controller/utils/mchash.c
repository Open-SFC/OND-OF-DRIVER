#include "controller.h"
#include "mchash.h"
#include "oflog.h"

int32_t mchash_table_create(uint32_t                 buckets,
                            uint32_t                 max_nodes,
                            mchash_free_func         free_func_cbk,
                            struct   mchash_table**  hash_table_p_p)
{
  struct mchash_table* hash_table_p;
  void*  hash_table_base_p;
  uint32_t mem_size = 0;
  uint32_t ii,jj;
  struct array_indirect_entry* node_p = NULL;
  struct mchash_bucket* bucket_p = NULL;

  //printf("%s %d %d\r\n",__FUNCTION__,max_nodes,buckets);
  
  cntlr_assert(buckets > 0);
  cntlr_assert(max_nodes > 0);
  cntlr_assert(free_func_cbk);

  /* calculate the memory required */
  mem_size = sizeof (struct mchash_table) + 
              ((buckets -1) * sizeof (struct mchash_bucket)) + X86_CACHE_LINE_SIZE;
  hash_table_p = (struct mchash_table *)calloc(1,mem_size);
  	
  if(hash_table_p == NULL)
    return MCHASHTBL_ERROR_NO_MEM;
  
  hash_table_base_p = hash_table_p;
  if((ulong)hash_table_p & (X86_CACHE_LINE_SIZE - 1))
    hash_table_p = (struct mchash_table *)(((ulong)hash_table_base_p + X86_CACHE_LINE_SIZE) & (~(X86_CACHE_LINE_SIZE - 1)));
  
  hash_table_p->table_base_p  = hash_table_base_p;
  hash_table_p->buckets       = buckets;
  hash_table_p->max_nodes     = max_nodes;
  hash_table_p->active_nodes  = 0;
  hash_table_p->passive_nodes = 0;
  hash_table_p->magic_no      = 1;
  hash_table_p->free_func_cbk = free_func_cbk;

  hash_table_p->indirection_array_p = (struct array_indirect_entry *)calloc(max_nodes, 
                                                                            sizeof(struct array_indirect_entry));  	
  if(hash_table_p->indirection_array_p == NULL)
  {
    free(hash_table_p);
    return MCHASHTBL_ERROR_NO_MEM;
  }

  CNTLR_LOCK_INIT(hash_table_p->mchash_table_lock);

  /* initialize the FreeQ  */
  /* Populate FreeQ */
  for(ii=0,node_p = (hash_table_p->indirection_array_p);ii<max_nodes;ii++)
  {
    if(ii != (max_nodes-1))	
	    node_p[ii].next_p = (struct mchash_dll_node *)&node_p[ii+1];
    else		
	    node_p[ii].next_p = NULL;		
    if(ii != 0)	
      node_p[ii].prev_p = (struct mchash_dll_node *)(&node_p[ii-1]);
    else		
      node_p[ii].prev_p = NULL;		
    node_p[ii].node_p = NULL;
    node_p[ii].magic = 0;
  }
  /* Link first and last nodes properly to the DLL Head(FreeQ) */
  hash_table_p->free_q.prev_p = (struct mchash_dll_node *) &hash_table_p->indirection_array_p[max_nodes -1];
  hash_table_p->free_q.next_p = (struct mchash_dll_node *) &hash_table_p->indirection_array_p[0];

  /* initialize the bucket lists */
  for (ii = 0; ii < hash_table_p->buckets; ii++)
  {
      bucket_p = &(hash_table_p->bucket_list[ii]);
      CNTLR_LOCK_INIT(bucket_p->mchash_bucket_lock); 
      bucket_p->head.prev_p = NULL;
      bucket_p->head.next_p = NULL;
      bucket_p->count = 0;
      for(jj = 0; jj < MCHASH_MAX_HASHBUCKET_CACHE_NODES; jj++)
        bucket_p->prefetch_nodes[jj] = 0;
  }
  *hash_table_p_p = hash_table_p;
  return MCHASHTBL_SUCCESS;
}

int32_t mchash_table_delete(struct mchash_table* hash_table_p)
{
  /* Applications are expected to delete all the hash table nodes before calling this API */

  if(hash_table_p->active_nodes)
  {
    printf("\r\n Active nodes =%d",hash_table_p->active_nodes);
    return MCHASHTBL_ERROR_NOT_EMPTY;
  }
  free(hash_table_p->indirection_array_p);
  free(hash_table_p->table_base_p);
  return MCHASHTBL_SUCCESS;
}
/***************************************************************************
 * Function Name : mchash_insert_node_in_middle 
 * Description   : Inserts a Node in a middle of given bucket of hash table 
 *                 
 * Input         : bucket_p - Bucket pointer 
 *                 prev_p   - Previous Node pointer 
 *                 node_p   - Node to be inserted, this node will be inserted 
 *                            between pPrev & pNext : pPrev<-->pNode<-->pNext
 *                 next_p   - Next Node pointer 
 * Output        : None 
 * Return value  : None
 ***************************************************************************/
void mchash_insert_node_in_middle(struct mchash_bucket* bucket_p,struct mchash_dll_node *prev_p,
                                  struct mchash_dll_node *node_p, struct mchash_dll_node* next_p)
{
  node_p->next_p = next_p;
  node_p->prev_p = prev_p;

  if(prev_p == NULL)
  {
    prev_p = &(bucket_p)->head;
  }
  CNTLR_RCU_ASSIGN_POINTER(prev_p->next_p, node_p);
  if(next_p == NULL)
  {
    next_p = &(bucket_p)->head;
  }
  CNTLR_RCU_ASSIGN_POINTER(next_p->prev_p,node_p);
  mchash_update_prefetch_nodes(bucket_p);
  bucket_p->count++;
  return;
}
/******************************************************************************/
