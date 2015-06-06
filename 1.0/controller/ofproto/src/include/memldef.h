#ifndef _MEM_LDEF_H
#define _MEM_LDEF_H

#if 0
#define CNTLR_DSMEMPOOL_STATIC_DEBUG 1
#endif

/***************************************************************************/
/*                  DATA STRUCTURE DEFINITIONS                             */
/***************************************************************************/
/* Memory pool data structure */
/* BitMap values */
#define MEMP_REFILL_PENDING   0x00000001 /* refilling is pending */
#define MEMP_INIT_TIME_REFILL 0x00000002 /* refilling is at init time */
#define CNTLR_MEMPOOL_DONOT_MEMSET 0x01

struct per_thread_mem_node
{
  uint8_t *per_thread_head_p; /* next free mem block & head of memblocks sll */
  uint32_t alloc_count;        /* Blocks allocated to application */
  uint32_t free_count;         /* Blocks available in this thread */
  uint32_t release_count;      /* Blocks Released to this thread */
};

struct mem_link_node
{   
  uint8_t* next_p;    /* points to the next memory block */
};

/* This structure is used to link the memory blocks in 'mmap' memory pool */
struct mempool_mmap_link_node
{
        struct mem_link_node    link_node; /* points to the next memory block */
        uint64_t	phys_addr; /* physical address of the memory block */
};

struct memory_pool
{
  struct cntlr_sll_q_node  next_p;          /* next pointer */
  struct cntlr_sll_q_node  refill_p;        /* pointer for refilling list */
  cntlr_lock_t   mempool_lock;
  struct    per_thread_mem_node* per_thread_list_p;
  uint32_t  block_count_for_moving_to_threadlist;
  uint8_t  *head_p; /* next free mem block & head of memblocks sll */
  uint8_t  *tail_p; /* tail of memblocks sll */
  uint32_t  bit_map; /* bit map */
  uint32_t  ref_cnt;
  uint8_t   del_b;
  uint8_t   lock_b; /* TRUE, if lock is wanted for memory pool */
  uint32_t  flags; 
  char mempool_name[MEMPOOL_MAX_POOL_NAME_LEN + 1];
  uint8_t   mempool_type; /* TRUE, if pool consists of mmap memory */
  struct mem_region_info    mem_region;
  unsigned long	kern_to_user_offset;

  /* Number of memory blocks available for Application usage */
  uint32_t  free_blocks_count;
  /* Number of memory blocks allocated to Application */
  uint32_t  allocated_blocks_count;
  /* Number of memory blocks released by Application */
  uint32_t  released_blocks_count;
  /* Number of failures in allocation of memory blocks */
  uint32_t  allocation_fail_blocks_count;
  /* Number of failures in releasing memory blocks */
  uint32_t  released_fail_blocks_count;
  /* Number of memory blocks that can still be allocated from heap */
  uint32_t  heap_free_blocks_count;
  /* Number of memory blocks allocated from heap to Application */
  uint32_t  heap_allocated_blocks_count;
  /* Number of memory blocks released by Application to heap */
  uint32_t  heap_released_blocks_count;
  /* Number of failures in allocation of memory blocks from heap */
  uint32_t  heap_allocation_fail_blocks_count;
  /* Number of memory blocks allocated from heap to Application */
  uint32_t  heap_allowed_blocks_count;
  /* maximum static blocks that can be allocated */
  uint32_t  static_blocks;
  /* Total Number of memory blocks */
  uint32_t  total_blocks_count;
  /* Size of each memory block */
  uint32_t  block_size;
  /* Maximum number of memory blocks that can be allocated and placed in internal lists of mempool */
  uint32_t  static_allocated_blocks_count;
  uint32_t  threshold_max;
  uint32_t  threshold_min;
  uint32_t  replenish_count;
};



typedef struct cntlr_mempool_info_s
{
   of_list_node_t           next_node; /** Next node of transport opend*/
   struct rcu_head          rcu_head; /** Used in RCU operation of transport list modications*/
   int32_t mempool_type;
   struct of_mempool_callbacks *mempool_callbacks;
} cntlr_mempool_info_t;

#define CNTLR_MEMPOOL_LISTNODE_OFFSET offsetof(cntlr_mempool_info_t, next_node)

#define MEMPOOL_PANIC(pMsg) \
{ \
  of_show_stack_trace();\
}

#define MEMP_PANIC(pMsg) \
{ \
  void *callstack[5];\
  int32_t i,frames=3;\
  char** strs;\
  of_show_stack_trace();\
  printf("\n\n ******* The block previously freed in the below path ********\n\n"); \
  callstack[0] = (void*) (*(uint32_t *)(mem_block_p+mempool_p->block_size+12)); \
  callstack[1] = (void*) (*(uint32_t *)(mem_block_p+mempool_p->block_size+8)); \
  callstack[2] = (void*) (*(uint32_t *)(mem_block_p+mempool_p->block_size+4)); \
  \
  strs = backtrace_symbols(callstack, frames);\
  for (i=0; i < frames; ++i)\
  {\
    printf("frame #%d: %s \r\n",i, strs[i]);\
  }\
  free(strs);\
  printf("\n *********************************\n\n"); \
}

#define MEMP_CHECK_STATIC_FREECNT_AND_REFILL(mempool_p) ((mempool_p->free_blocks_count <=  \
                                                          mempool_p->threshold_min) && \
                                                         (mempool_p->static_allocated_blocks_count < \
                                                          mempool_p->total_blocks_count)) 

/******************************************************************************/
#ifdef CNTLR_DSMEMPOOL_STATIC_DEBUG 
/* CNTLR_DSMEMPOOL_STATIC_DEBUG macro is used to find static pool memory 
 * corruption  or double free. 8 bytes extra memory is allocated pre and 
 * post to the application date size, and certain manipulation is done on 
 * those extra bytes to find any corruption or double free
 */

#define MEMP_STATIC_PRE_SIGNATURE_BYTES         4
#define MEMP_STATIC_POST_SIGNATURE_BYTES        4

#define MEMP_ACTIVE_PRE_SIGNATURE          0x5779fc5b 
#define MEMP_ACTIVE_POST_SIGNATURE         0x98d7625b
#define MEMP_INACTIVE_PRE_SIGNATURE        0x0a1b2c3d 
#define MEMP_INACTIVE_POST_SIGNATURE       0x4e5f689e

#define MEMP_DATA_EXP             0x6b

#define MEMP_SET_PRE_SIGNATURE(ptr,val) \
{ \
  *((uint32_t *)((ptr) - MEMP_STATIC_PRE_SIGNATURE_BYTES)) = val; \
}

#define MEMP_SET_POST_SIGNATURE(ptr,val,mempool_p) \
{ \
  *((uint32_t *)((ptr)+mempool_p->block_size)) = val; \
}

#define MEMP_GET_PRE_SIGNATURE(ptr) \
                         *((uint32_t *)((ptr)-MEMP_STATIC_PRE_SIGNATURE_BYTES))

#define MEMP_GET_POST_SIGNATURE(ptr,mempool_p) \
        *((uint32_t *)((ptr)+mempool_p->block_size))
/***************************************************************************/
/* This macro is called in 'mempool_release_mem_block' to check debug info */
/***************************************************************************/
#define MEMP_CHECK_DEBUG_INFO_IN_STATIC_BLOCK(mem_block_p) \
{ \
  /* Check the pre and post signature(double free check)*/ \
  if((MEMP_GET_PRE_SIGNATURE(mem_block_p) == MEMP_INACTIVE_PRE_SIGNATURE) || \
     (MEMP_GET_POST_SIGNATURE(mem_block_p,mempool_p) == MEMP_INACTIVE_POST_SIGNATURE)) \
  { \
    printf("\n\n ************ DOUBLE FREE *************\n"); \
    printf(" %s: Double free of static block happend for mempool %s \n", \
           __FUNCTION__,mempool_p->mempool_name); \
    MEMP_PANIC("***** DOUBLE FREE ******"); \
    exit(0); \
  } \
  /* Check the pre signature(memory overwritten check) */ \
  if(MEMP_GET_PRE_SIGNATURE(mem_block_p) != MEMP_ACTIVE_PRE_SIGNATURE) \
  { \
    printf("\n\n ******* MEMORY BLOCK(pre signature) OVERWRITTEN *****\n"); \
    printf(" %s: Mempool block(pre signature) overwritten for mempool %s \n", \
           __FUNCTION__,mempool_p->mempool_name); \
    MEMPOOL_PANIC("***** MEMORY BLOCK(pre signature) OVERWRITTEN ******"); \
    exit(0); \
  } \
  /* Check the post signature(memory overwritten check) */ \
  if(MEMP_GET_POST_SIGNATURE(mem_block_p,mempool_p) != MEMP_ACTIVE_POST_SIGNATURE) \
  { \
    printf("\n\n **** MEMORY BLOCK(post signature) OVERWRITTEN ****\n"); \
    printf(" %s: Mempool block(post signature) overwritten for mempool %s \n", \
           __FUNCTION__,mempool_p->mempool_name); \
    MEMPOOL_PANIC("***** MEMORY BLOCK(post signature) OVERWRITTEN ******"); \
    exit(0); \
  } \
  /* Reset the pre and post signatures*/ \
  MEMP_SET_PRE_SIGNATURE(mem_block_p,MEMP_INACTIVE_PRE_SIGNATURE); \
  MEMP_SET_POST_SIGNATURE(mem_block_p,MEMP_INACTIVE_POST_SIGNATURE,mempool_p); \
}
#if 0
  *((uint32_t *)(mem_block_p + (mempool_p->block_size+4))) = (uint32_t)__builtin_return_address(2); \
  *((uint32_t *)(mem_block_p+mempool_p->block_size+8)) = (uint32_t)__builtin_return_address(1); \
  *((uint32_t *)(mem_block_p+mempool_p->block_size+12)) = (uint32_t)__builtin_return_address(0); \
}
#endif
/***********************************************************************/
/* This macro is called in 'CNTLRMemPoolGetBlock' to insert debug info */
/***********************************************************************/
#define MEMP_INSERT_DEBUG_INFO_IN_STATIC_BLOCK(mem_block_p) \
{ \
  /* Check the pre and post signature */ \
  if((MEMP_GET_PRE_SIGNATURE(mem_block_p) != MEMP_INACTIVE_PRE_SIGNATURE) || \
     (MEMP_GET_POST_SIGNATURE(mem_block_p,mempool_p) != MEMP_INACTIVE_POST_SIGNATURE)) \
  { \
    printf("\n\n ************ MEMORY OVERWRITTEN *************\n"); \
    printf(" %s(line:%d): static block signatures overwritten of MemPool %s \n", \
           __FUNCTION__,__LINE__,mempool_p->mempool_name); \
    if((MEMP_GET_PRE_SIGNATURE(mem_block_p) != MEMP_INACTIVE_PRE_SIGNATURE) )\
      printf(" Pre - signature Overwritten with ***  0x%x ***** \r\n",MEMP_GET_PRE_SIGNATURE(mem_block_p));\
    if((MEMP_GET_POST_SIGNATURE(mem_block_p,mempool_p) != MEMP_INACTIVE_POST_SIGNATURE) )\
      printf(" Post - signature Overwritten with ***  0x%x ***** \r\n",MEMP_GET_POST_SIGNATURE(mem_block_p,mempool_p));\
     /*MEMP_PANIC(" ***** MEMORY OVERWRITTEN ******  ")*/ \
     MEMPOOL_PANIC("***** MEMORY OVERWRITTEN ******"); \
    exit(0); \
  } \
  \
  /* Set the pre and post signature */ \
  MEMP_SET_PRE_SIGNATURE(mem_block_p,MEMP_ACTIVE_PRE_SIGNATURE); \
  MEMP_SET_POST_SIGNATURE(mem_block_p,MEMP_ACTIVE_POST_SIGNATURE,mempool_p); \
  if(!(mempool_p->flags & CNTLR_MEMPOOL_DONOT_MEMSET)) \
  { \
    memset(mem_block_p, 0, mempool_p->block_size); \
  } \
}
#else
#define MEMP_CHECK_DEBUG_INFO_IN_STATIC_BLOCK(mem_block_p) 
#define MEMP_INSERT_DEBUG_INFO_IN_STATIC_BLOCK(mem_block_p) 
#endif /* CNTLR_DSMEMPOOL_STATIC_DEBUG */
/***************************************************************************/

/***************************************************************************/
/*                      GLOBAL VARIABLE DEFINITIONS                        */
/***************************************************************************/
extern struct cntlr_sll_q   mempool_list_g; /* Linked list memory pools */
/***************************************************************************/
/*                      FUNCTION PROTOTYPE DECLERATIONS                    */
/***************************************************************************/

/*Function prototypes*/
int32_t mempool_library_init(void);


/***************************************************************************/
/*                      MACRO DEFINITIONS                                  */
/***************************************************************************/
#define CNTLR_MEM_POOL_CNT        CNTLR_SLLQ_COUNT (&MemPoolList_g)

#define MEM_POOL_INC_REFCNT(mempool_p) (mempool_p->ref_cnt++)
#define MEM_POOL_DEC_REFCNT(mempool_p) (mempool_p->ref_cnt--)
#define MEM_POOL_IS_REFCNT_ZERO(mempool_p) \
                    (mempool_p->ref_cnt == 0) ? TRUE : FALSE
#define MEM_POOL_SET_BDEL(mempool_p) mempool_p->del_b |= TRUE
#define MEM_POOL_CHECK_BDEL(mempool_p) \
                    (mempool_p->del_b == TRUE) ? TRUE : FALSE
#define MEM_POOL_CHECK_BDEL_NOREF(mempool_p) \
                    ((mempool_p->del_b == TRUE) && (mempool_p->ref_cnt == 0)) \
                    ? TRUE : FALSE
#endif /* _MEM_LDEF_H */
