/* useful file */

#include "controller.h"
#include "timerapi.h"
#include "memapi.h"
#include <sys/time.h>

#define CONFIG_CNTLR_BOARD_X86 1
#define MICROSECS_PER_SEC 1000000
#define MILLISECS_PER_SEC 1000
#define REDISTRIBUTION_REQUIRED 2
#define REDISTRIBUTION_NOT_REQUIRED 3

extern uint32_t*  tmr_min_gran_g;
#define __timer_get_least_granularity() tmr_min_gran_g[cntlr_thread_index]

struct tmr_node
{
  struct tmr_node* next_p;
  struct tmr_node* prev_p;

  uint32_t     thread_id;  /* 0 for Non worker thread */
  uint8_t      heap_b;
  void*        bucket_p;  
  uint32_t     index;
  uint32_t     magic;
  char         is_residual_b;
  tmr_time_t   time_left; 
  uint8_t      timer_stopped_b; 
  uint8_t      timer_expired_b; 
  uint8_t      periodic_b;
  uint8_t      periodic_timer_stopped_b;
  uint64_t     time_out;
  tmr_cbk_func routine_p;
  void*        arg1;
  void*        arg2;
};

struct tmr_bucket
{
  uint32_t no_of_nodes; /* Number of Timer nodes present in the current bucket. */
  uint8_t  in_expiry_b;
  struct   tmr_node*  head_p;
};

struct tmr_wheel
{
  uint32_t size_of_wheel;
  uint32_t bucket_gap;
  uint32_t bucket_id;
  uint32_t range_of_wheel;
  struct   tmr_bucket* bucket_p;  /* Array of buckets */
};

struct tmr_container
{
  uint32_t  container_id_incont;
  uint32_t  no_of_tmr_nodes_incont;  
  uint32_t  no_of_wheels;

#ifdef CONFIG_CNTLR_BOARD_X86
  uint64_t  time_last_expiry;
#else
  struct    timeval time_last_expiry;
#endif

 struct        tmr_wheel* wheel_p;  //Aray of Wheels  to make it cascaded wheel timer
 cntlr_lock_t  tmr_container_lock;
};

struct tmr_config
{
  struct     tmr_container***  container_head_p_p_p;
  uint32_t   no_of_containers;
  cntlr_lock_t tmr_config_lock;
};

struct tmr_rcu_cont_list
{
  struct   tmr_rcu_cont_list* next_p;
  struct   tmr_rcu_cont_list* prev_p;
  uint32_t container_id;
};

struct tmr_cont_and_node_saferef
{
  struct  saferef container_saferef;
  struct  tmr_saferef tmr_node_saferef;
  struct  tmr_cont_and_node_saferef* next_p;
  uint8_t heap_b;
};

struct tmr_stop_list
{
  struct     tmr_cont_and_node_saferef* head_p;
  struct     tmr_cont_and_node_saferef* tail_p;
  cntlr_lock_t tmr_stop_list_mutex;
  uint32_t   tmr_stop_count;
};

int32_t   timer_module_init(void);
void      timer_process_timers(void);
uint32_t  timer_get_least_granularity(void);
int32_t  timer_stop_tmr(struct saferef* container_saferef_p, struct tmr_saferef* tmr_node_saferef_p);
int32_t timer_restart_tmr(struct saferef* container_saferef_p, struct tmr_saferef* tmr_saferef_p, uint8_t periodic_b, tmr_time_t time_out,tmr_cbk_func func, void* arg1_p,void* arg2_p);
/**********************************************************************/
