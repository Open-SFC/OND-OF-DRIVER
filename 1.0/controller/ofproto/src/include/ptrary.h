
#define PTRARY_SUCCESS  0
#define PTRARY_FAILURE -1

typedef void (* cntlr_ptr_ary_free_func)(void *ptr_p);

struct cntlr_ptr_ary_node
{
  /* NOTE: must be first two fields in this structure.
   * free_q and active_q uses circular linked list with head nodes containing only these two fields.
   * MUST match with cntlr_ptr_ary_head_node node. */
  struct cntlr_ptr_ary_node *prev_p;
  struct cntlr_ptr_ary_node *next_p;

  void     *ptr_p;
  uint32_t magic;
}; 

/* node used as head node of free_q and active_q circular lists */
struct cntlr_ptr_ary_head_node
{
  struct cntlr_ptr_ary_node  *prev_p;
  struct cntlr_ptr_ary_node  *next_p;
};

enum {
  CNTLR_PTR_ARY_FLAG_LOCK         = 1 /*BIT0  */,
  CNTLR_PTR_ARY_FLAG_ARY_MEM_HEAP = 2 /*BIT31 */, /* heap memory allocated by ptrary module itself */
};

struct cntlr_ptr_ary
{
  struct cntlr_ptr_ary_node *base_p;
  uint32_t           magic;
  uint32_t           flags;
  cntlr_lock_t       lock;
  struct cntlr_ptr_ary_node free_q;   /* circular doubly linked list of free/unallocated nodes */
  struct cntlr_ptr_ary_node active_q; /* circular doubly linked list of active nodes */
  uint32_t           count;           /* number of entries given during initialization */

#ifdef CONFIG_CNTLR_PTRARY_DEBUG_STATS 
struct stats
  {
    cntlr_lock_t lock;
    uint32_t  inserts;
    uint32_t  insert_failures;
    uint32_t  removals;
    uint32_t  removal_failures;
  };
};  

#define CNTLR_PTRARY_SINC(name) do { \
                                   CNTLR_LOCK_TAKE(ptr_ary_p->stats.lock); \
                                   ptr_ary_p->stats.ui##name++; \
                                   CNTLR_LOCK_RELEASE(ptr_ary_p->stats.lock); \
                                 } while(0)
#else
#define CNTLR_PTRARY_SINC(name) do {} while(0)
#endif
};

#define CNTLR_PTR_ARY_LOCK(ptr_ary_p) do { \
               if(ptr_ary_p->flags & CNTLR_PTR_ARY_FLAG_LOCK) CNTLR_LOCK_TAKE(ptr_ary_p->lock); \
             } while(0) 

#define CNTLR_PTR_ARY_UNLOCK(ptr_ary_p) do { \
               if(ptr_ary_p->flags & CNTLR_PTR_ARY_FLAG_LOCK) CNTLR_LOCK_RELEASE(ptr_ary_p->lock); \
             } while(0) 

#define cntlr_ptr_ary_get_ptr_ex(ptr_ary_p,this_index,this_magic)  (((ptr_ary_p)->base_p[this_index].magic == this_magic)?(ptr_ary_p)->base_p[this_index].ptr_p:NULL)
#define cntlr_ptr_ary_get_ptr(ptr_ary_p,saferef)  cntlr_ptr_ary_get_ptr_ex(ptr_ary_p,(saferef).index,(saferef).magic)

/* caller should take care of comparing with magic number.
 * typically used in conjunction with cntlr_ptr_ary_get_ptr_by_index API
 */
#define cntlr_ptr_ary_get_ptr_by_index(ptr_ary_p,this_index)  ((ptr_ary_p)->base_p[this_index].ptr_p)

int cntlr_ptr_ary_init(struct cntlr_ptr_ary* ptr_ary_p);

int cntlr_ptr_ary_init(struct cntlr_ptr_ary *ptr_ary_p)
{
  int32_t ii;
  struct cntlr_ptr_ary_node *node_p;

  cntlr_assert(ptr_ary_p);
  cntlr_assert(ptr_ary_p->count > 0);

  /* Allocate memory if user did not allocate */
  if(!ptr_ary_p->base_p)
  {
    ptr_ary_p->base_p = (struct cntlr_ptr_ary_node*) malloc(ptr_ary_p->count*sizeof(struct cntlr_ptr_ary_node));
    if(ptr_ary_p->base_p)
      ptr_ary_p->flags |= CNTLR_PTR_ARY_FLAG_ARY_MEM_HEAP;
    else
      return PTRARY_FAILURE;
  }
  ptr_ary_p->magic = 1;
  CNTLR_LOCK_INIT(ptr_ary_p->lock);
#ifdef CONFIG_CNTLR_PTRARY_DEBUG_STATS
  CNTLR_LOCK_INIT(ptr_ary_p->stats.lock);
#endif

  ptr_ary_p->active_q.prev_p = ptr_ary_p->active_q.next_p = (struct cntlr_ptr_ary_node*) &ptr_ary_p->active_q;

  /* 
   * free_q node is not a valid node and just serves the
   * purpose of circular linked list
   */
  ptr_ary_p->free_q.magic = 0;
  ptr_ary_p->free_q.ptr_p = NULL;

  /* Populate free_q */
  for(ii=0,node_p=ptr_ary_p->base_p;ii<ptr_ary_p->count;ii++)
  {
    node_p->next_p = node_p+1;
    node_p->prev_p = node_p-1;
    node_p->ptr_p = NULL;
    node_p->magic = 0;
    node_p++;
  }
  /* Link first and last nodes properly to the circular list */
  ptr_ary_p->base_p[0].prev_p = &ptr_ary_p->free_q;
  ptr_ary_p->free_q.prev_p = &ptr_ary_p->base_p[ptr_ary_p->count-1];
  ptr_ary_p->base_p[ptr_ary_p->count-1].next_p = &ptr_ary_p->free_q;
  ptr_ary_p->free_q.next_p = &ptr_ary_p->base_p[0];
  return PTRARY_SUCCESS;
}

static inline int32_t cntlr_ptr_ary_deinit(struct cntlr_ptr_ary* ptr_ary_p)
{
  if(ptr_ary_p->flags & CNTLR_PTR_ARY_FLAG_ARY_MEM_HEAP)
  {
     free(ptr_ary_p->base_p);
  }
  return PTRARY_SUCCESS;
}

/* Inserts ptr_p in the ptr_ary_p and return saferef_p which can be used by any module
 * to safely refer the inserted pointer.
 * This function return PTRARY_SUCCESS upon successful insertion and return PTRARY_FAILURE
 * upon failure.
 */
static inline int32_t cntlr_ptr_ary_insert(struct cntlr_ptr_ary* ptr_ary_p, void *ptr_p, struct saferef *saferef_p)
{
  struct cntlr_ptr_ary_node *node_p;   

  cntlr_assert(ptr_ary_p);
  cntlr_assert(ptr_p);
  cntlr_assert(saferef_p);

  CNTLR_PTR_ARY_LOCK(ptr_ary_p);
  if(ptr_ary_p->free_q.next_p != &ptr_ary_p->free_q)
  {
    /* Remove a node from free_q */
    node_p = ptr_ary_p->free_q.next_p;
    ptr_ary_p->free_q.next_p = node_p->next_p;
    node_p->next_p->prev_p = &ptr_ary_p->free_q;
    node_p->magic = ptr_ary_p->magic; 
    ptr_ary_p->magic++;
    if(0 == ptr_ary_p->magic) ptr_ary_p->magic = 1;
    node_p->ptr_p = ptr_p;

    /* Insert in active_q */
    node_p->prev_p = (struct cntlr_ptr_ary_node*)&ptr_ary_p->active_q;
    node_p->next_p = ptr_ary_p->active_q.next_p;
    ptr_ary_p->active_q.next_p = node_p;
    node_p->next_p->prev_p = node_p;
    CNTLR_PTR_ARY_UNLOCK(ptr_ary_p);

    saferef_p->magic = node_p->magic;
    saferef_p->index = node_p - (ptr_ary_p->base_p);
    CNTLR_PTRARY_SINC(inserts);
    return PTRARY_SUCCESS;
  }
  CNTLR_PTR_ARY_UNLOCK(ptr_ary_p);
  CNTLR_PTRARY_SINC(insert_failures);
  return PTRARY_FAILURE;
}

static inline int32_t cntlr_ptr_ary_re_insert(struct cntlr_ptr_ary* ptr_ary_p, struct saferef* saferef_p)
{
  struct cntlr_ptr_ary_node *node_p = &(ptr_ary_p->base_p[saferef_p->index]);

  CNTLR_PTR_ARY_LOCK(ptr_ary_p);
  if(saferef_p->magic != node_p->magic)
  {
    CNTLR_PTR_ARY_UNLOCK(ptr_ary_p);
    return PTRARY_FAILURE;
  }
  node_p->magic = ptr_ary_p->magic;
  ptr_ary_p->magic++;
  saferef_p->magic = node_p->magic;
  CNTLR_PTR_ARY_UNLOCK(ptr_ary_p);
  return PTRARY_SUCCESS;
}

/* Inserts ptr_p in the ptr_ary_p at a given index and return saferef_p which can be used by any module
 * to safely refer the inserted pointer. The insertion fails if the index is already in use.
 * This function return PTRARY_SUCCESS upon successful insertion and return PTRARY_FAILURE
 * upon failure.
 */
static inline int32_t cntlr_ptr_ary_insert_at_index(struct cntlr_ptr_ary* ptr_ary_p, void *ptr_p,
                                                uint32_t index, struct saferef* saferef_p)
{
  struct cntlr_ptr_ary_node *node_p;   

  cntlr_assert(ptr_ary_p);
  cntlr_assert(ptr_p);
  cntlr_assert(saferef_p);
  cntlr_assert(index < ptr_ary_p->count);

  CNTLR_PTR_ARY_LOCK(ptr_ary_p);
  node_p = &ptr_ary_p->base_p[index];
  if(0 == node_p->magic) /* not in use */
  {
    /* Remove from free_q */
    node_p->prev_p->next_p = node_p->next_p;
    node_p->next_p->prev_p = node_p->prev_p;

    node_p->magic = ptr_ary_p->magic;
    ptr_ary_p->magic++;
    if(0 == ptr_ary_p->magic) ptr_ary_p->magic = 1;
    node_p->ptr_p = ptr_p;

    /* Insert in active_q */
    node_p->prev_p = (struct cntlr_ptr_ary_node*)&ptr_ary_p->active_q;
    node_p->next_p = ptr_ary_p->active_q.next_p;
    ptr_ary_p->active_q.next_p = node_p;
    node_p->next_p->prev_p = node_p;

    CNTLR_PTR_ARY_UNLOCK(ptr_ary_p);
    saferef_p->magic = node_p->magic;
    saferef_p->index = node_p - (ptr_ary_p->base_p);
    CNTLR_PTRARY_SINC(inserts);
    return PTRARY_SUCCESS;
  }
  CNTLR_PTR_ARY_UNLOCK(ptr_ary_p);
  CNTLR_PTRARY_SINC(insert_failures);
  return PTRARY_FAILURE;
}

static inline int32_t cntlr_ptr_ary_remove_ex(struct cntlr_ptr_ary* ptr_ary_p, uint32_t index, uint32_t magic)
{
  struct cntlr_ptr_ary_node *node_p = &(ptr_ary_p->base_p[index]);
  //void *ptr_p;

  cntlr_assert(index < ptr_ary_p->count);
  cntlr_assert(magic); /* invalid magic number */

  CNTLR_PTR_ARY_LOCK(ptr_ary_p);

  //cntlr_assert(node_p->ptr_p); /* should have a valid pointer stored ? */
  if(magic != node_p->magic) /* attempting to remove already removed pointer? */
  {
    CNTLR_PTR_ARY_UNLOCK(ptr_ary_p);
    CNTLR_PTRARY_SINC(removal_failures);
    return PTRARY_FAILURE;
  }

  node_p->magic = 0; /* reset the magic first so new readers will not see the pointer any more */ 
  //ptr_p = node_p->ptr_p;
  node_p->ptr_p = NULL;

  /* Remove from active_q */
  node_p->prev_p->next_p = node_p->next_p;
  node_p->next_p->prev_p = node_p->prev_p;

  /* Insert in free_q */
  node_p->next_p = ptr_ary_p->free_q.next_p;
  node_p->prev_p = &ptr_ary_p->free_q;
  node_p->next_p->prev_p = node_p;
  ptr_ary_p->free_q.next_p = node_p;

  CNTLR_PTR_ARY_UNLOCK(ptr_ary_p);
  CNTLR_PTRARY_SINC(removals);
  return PTRARY_SUCCESS;
}

#define cntlr_ptr_ary_remove(ptr_ary_p,saferef) cntlr_ptr_ary_remove_ex(ptr_ary_p,(saferef).index,(saferef).magic)
