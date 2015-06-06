#include "tmrldef.h" 
#include "ptrary.h" 

//#define CNTLR_TIMER_DEBUG

struct        tmr_config timer_config_g;
void*         tmr_mempool_g;
uint32_t*     tmr_min_gran_g;  /*Maintained per thread just to avoid locks TODO Will atomic variable sufficient*/
void*         tmr_cont_and_node_saferef_mempool_g;
struct        tmr_stop_list* tmr_stop_list_g;
cntlr_lock_t  tmr_container_lock;

struct tmr_rcu_cont_list* rcu_cont_list_head_g;
cntlr_lock_t  rcu_cont_list_lock_g;
struct cntlr_ptr_ary container_ptr_ary_g = 
{
  .count = TIMER_MAX_NO_OF_CONTAINERS
};
struct   cntlr_ptr_ary* tmr_node_ptr_ary_g;
struct   saferef saferef;
struct   saferef def_cont_saferef;
uint32_t tmr_trace_id_g;

extern inline int32_t cntlr_get_time(uint64_t *timecnt);

extern __thread uint32_t cntlr_thread_index;

/*FUNCTION_PROTO_START  *************************************************/
uint32_t tmr_num_of_wheels(uint32_t granularity, uint32_t range);
void     tmr_delete_container_with_id(uint32_t container_id);
int32_t  tmr_del_from_rcu_list(struct tmr_container* del_container_p,uint8_t flag_b);
int32_t  redistribute_nodes(struct tmr_container* container_p, uint16_t wheel_index, struct tmr_wheel* wheel_p);
void     tmr_enque_stop(struct tmr_stop_list* tmr_list,struct tmr_cont_and_node_saferef* tmr_stop_node_p);
void     tmr_deque_stop(struct tmr_stop_list* tmr_list_p);
void     tmr_insert_periodic_tmr(struct tmr_node* node_p,struct tmr_container* container_p);
int32_t timer_init_container(uint32_t granularity,uint32_t max_timers ,uint32_t range,struct saferef* container_saferef);
int32_t tmr_add_to_rcu_list(int32_t container_id);
int32_t timer_delete_default_container(struct saferef* container_saferef_p ,uint8_t flag_b);
void timer_module_deinit(void);
void timer_get_tmr_info(void);
/*FUNCTION_PROTO_END  ******************************************************/

/***************************************************************************/
/*
 * Function Name:timer_module_init
 *
 * Description:Timer initialization API , it involves resetting of global data,
 * initialization of mempools, and creation of default container.
 *
 * Parameters: NONE
 *
 * Return values: TIMER_SUCCESS In case of successfull initialization.
 * TIMER_FAILURE In case of any error condition.
 */
/***************************************************************************/
int32_t timer_module_init(void)
{
  uint32_t ii,jj;
  uint32_t total_entries,max_static_entries;
  uint32_t granularity,range;
  int32_t  ret_val = TIMER_FAILURE;
  struct mempool_params mempool_info={};

  /* Reset the global variables */
  memset(&timer_config_g,0,sizeof(struct tmr_config)); 
  CNTLR_LOCK_INIT(timer_config_g.tmr_config_lock);
  CNTLR_LOCK_INIT(rcu_cont_list_lock_g);


  timer_config_g.container_head_p_p_p = (struct tmr_container ***)calloc(TIMER_MAX_NO_OF_CONTAINERS,sizeof(struct tmr_container *));
  if(timer_config_g.container_head_p_p_p == NULL)    
  {
    printf("Memory allocation failed %s %d \r\n",__FUNCTION__,__LINE__);
    return TIMER_FAILURE;
  }
  tmr_min_gran_g =(uint32_t *)calloc(CNTLR_NO_OF_THREADS_IN_SYSTEM+1,sizeof(uint32_t));
  if(tmr_min_gran_g == NULL)
  {
    printf("Allocation for minimum granularity failed  %s %d\r\n",__FUNCTION__,__LINE__);
    return TIMER_FAILURE;
  }
  for(ii=0;ii<CNTLR_NO_OF_THREADS_IN_SYSTEM+1;ii++) 
    tmr_min_gran_g[ii]=0xffffffff;
  if(cntlr_ptr_ary_init(&container_ptr_ary_g) != TIMER_SUCCESS)
  {
    return TIMER_FAILURE;
  }
  /* Allocate memory for timer stop list, Initialize for all the threads*/
  tmr_stop_list_g = (struct tmr_stop_list *)calloc(CNTLR_NO_OF_THREADS_IN_SYSTEM+1,sizeof(struct tmr_stop_list));
  if(tmr_stop_list_g == NULL) 
  {
    printf("Timerstop list memory allocation failed!\n");
    return TIMER_FAILURE;
  }
  for(ii=0;ii< CNTLR_NO_OF_THREADS_IN_SYSTEM+1;ii++)
  {
    tmr_stop_list_g[ii].head_p = NULL;
    tmr_stop_list_g[ii].tail_p = NULL;
    tmr_stop_list_g[ii].tmr_stop_count = 0;
    CNTLR_LOCK_INIT(tmr_stop_list_g[ii].tmr_stop_list_mutex);
  }
  for(ii=0;ii< TIMER_MAX_NO_OF_CONTAINERS;ii++)
  {
    timer_config_g.container_head_p_p_p[ii] = (struct tmr_container **)calloc(CNTLR_NO_OF_THREADS_IN_SYSTEM+1,sizeof(struct tmr_container *));
    if(timer_config_g.container_head_p_p_p[ii] == NULL)
    {
      printf("Memory allocation failed %s %d \r\n",__FUNCTION__,__LINE__);
      goto ERROR;
    }
  }
  /* Initialize default timer container */
  granularity = TIMER_DEFAULT_CONTAINER_TIMER_GRANULARITY;
  range       = TIMER_DEFAULT_CONTAINER_TIMER_RANGE;

  ret_val = timer_init_container(granularity,0,range,&def_cont_saferef);

  if(ret_val == TIMER_FAILURE)
    goto ERROR;
  printf("Timer Initialization successful \r\n");

  total_entries      = TIMER_DEFAULT_CONTAINER_TIMER_ENTRIES_MAX;
  max_static_entries = TIMER_DAFAULT_CONTAINER_TIMER_STATIC_ENTRIES_MAX; 

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
  mempool_info.block_size = sizeof(struct tmr_node);
  mempool_info.max_blocks = total_entries;
  mempool_info.static_blocks = max_static_entries;
  mempool_info.threshold_min = max_static_entries/10;
  mempool_info.threshold_max = max_static_entries/3;
  mempool_info.replenish_count = max_static_entries/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  TRUE;
  mempool_info.noof_blocks_to_move_to_thread_list = (max_static_entries/(CNTLR_NO_OF_THREADS_IN_SYSTEM+1));

  ret_val = mempool_create_pool("TMR_X86_TIMER", &mempool_info,
			  &tmr_mempool_g);

  if(ret_val != MEMPOOL_SUCCESS)
  {
    printf("\r\n Failed to create Timer mempool\n");
    goto ERROR;
  }

  tmr_node_ptr_ary_g = (struct cntlr_ptr_ary *)calloc(CNTLR_NO_OF_THREADS_IN_SYSTEM+1,sizeof(struct cntlr_ptr_ary)); 
  if(tmr_node_ptr_ary_g == NULL)    
  {
    printf("Memory allocation failed %s %d \r\n",__FUNCTION__,__LINE__);
    ret_val = TIMER_FAILURE;
    goto ERROR;
  }

  /*Non Worker thread case we need to take lock*/
  tmr_node_ptr_ary_g[0].flags = CNTLR_PTR_ARY_FLAG_LOCK;
  for(ii=0;ii<CNTLR_NO_OF_THREADS_IN_SYSTEM+1;ii++)   
  {
    tmr_node_ptr_ary_g[ii].count = (max_static_entries)/(CNTLR_NO_OF_THREADS_IN_SYSTEM+1);  //Reference for ABI TODO where should we get this input from 
    if(cntlr_ptr_ary_init(&tmr_node_ptr_ary_g[ii]) != TIMER_SUCCESS)
    {    
      for(jj=0;jj< ii;jj++)
      {
        cntlr_ptr_ary_deinit(&tmr_node_ptr_ary_g[jj]);
      }
      free(tmr_node_ptr_ary_g);
      return TIMER_FAILURE;
    }
  }
  /* Create mempool to handle the timer stop events from different thread*/
  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
  mempool_info.block_size = sizeof(struct tmr_cont_and_node_saferef);
  mempool_info.max_blocks = total_entries;
  mempool_info.static_blocks = max_static_entries;
  mempool_info.threshold_min = max_static_entries/10;
  mempool_info.threshold_max = max_static_entries/3;
  mempool_info.replenish_count = max_static_entries/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  TRUE;
  mempool_info.noof_blocks_to_move_to_thread_list = (max_static_entries/(CNTLR_NO_OF_THREADS_IN_SYSTEM+1));

  ret_val = mempool_create_pool("TMR_X86_TIMER_STOP_NODE", &mempool_info,
		  &tmr_cont_and_node_saferef_mempool_g); 

  if(ret_val != MEMPOOL_SUCCESS)
  {
    printf("Timer Stop List Mempool Init Failed  %s %d  \r\n",__FUNCTION__,__LINE__);
    return TIMER_FAILURE;
  }
  return TIMER_SUCCESS;
ERROR: 
  for(ii=0;ii< TIMER_MAX_NO_OF_CONTAINERS;ii++)
  {
    if(timer_config_g.container_head_p_p_p[ii])
      free(timer_config_g.container_head_p_p_p[ii]);
  }
  free(timer_config_g.container_head_p_p_p);
  free(tmr_min_gran_g);
  cntlr_ptr_ary_deinit(&container_ptr_ary_g);
  mempool_delete_pool(tmr_mempool_g);
  free(tmr_stop_list_g);
  return ret_val;
}
/** \ingroup NONDPAATIMER

  <b>Description:</b>\n
   This function is used to create a timer container. It involves computing
   the number of wheels, number of buckets in each wheel and creating the timer
   instances. One timer instance per worker thread and one timer instance for
   all the non worker threads.

  <b>Input Parameters:</b>\n
  <i>ulGranularity    :</i> Granularity of the timer.
  <i>ulMaxTimers      :</i> This field is used only for DPAA timers, N/A here.
  <i>ulRange          :</i> Range of the timers.

  <b>Output Parameters</b>\n
  <i>pContainerSafeRef:</i> Pointer to safe reference of the container that is created.

  <b>Returns</b>\n
  <i> TIMER_SUCCESS </i> In case of successfull creation of container.
  <i> TIMER_FAILURE </i> In case of any error condition.
 */
/***************************************************************************/
/*
 * Function Name:timer_init_container
 *
 * Description: This function computes the number of wheels and creates the timer
 * instances for all the worker threads and one timer instance for house keeping
 * thread to support timers for non worker threads.
 *
 * Parameters: 
 * ulGranularity (I) Granularity of the timer.
 * ulMaxTimers (I) This field is used only for DPAA timers, N/A here.
 * ulRange (I) Range of the timers.
 * pContainerSafeRef (O) Pointer to safe reference of the container that is created.
 *
 * Return values: 
 * TIMER_FAILURE In case of error conditions. Other wise TIMER_SUCCESS
 */
/***************************************************************************/
int32_t timer_init_container(uint32_t granularity,uint32_t max_timers ,uint32_t range,struct saferef* container_saferef)
{
  uint16_t no_of_wheels,ii,jj,kk,ll;
  uint32_t factor,no_of_buckets,rounded_no_of_buckets;
  struct   tmr_container* container_p;
  struct   tmr_wheel* wheel_p;
  struct   tmr_bucket* bucket_p;
  uchar8_t  abort_b = FALSE;

  uint32_t tmp_range;
  /* Validate input parameters */
  if(granularity < 10 || range > 0xffffffffffffffffULL)  //TODO should high end now checked 
  {
    printf("TIMER CONFIGURATION OUT OF RANGE 0- 2POW64");
    return TIMER_FAILURE;
  }

  /* Compute number of wheels */
  no_of_wheels = tmr_num_of_wheels(granularity,range);
  if(no_of_wheels < 1)
  {
    printf("NUMBER OF WHEELS LESS THAN 1");
    return TIMER_FAILURE;
  }

  /* Maintain a global variable to count number of containers with lock, Usefull when adding containers in middle */
  CNTLR_LOCK_TAKE(timer_config_g.tmr_config_lock);
  if(timer_config_g.no_of_containers == TIMER_MAX_NO_OF_CONTAINERS)
  {
    printf("CONTAINER CANNOT BE CREATED, TIMER_MAX_NO_OF_CONTAINERS REACHED");
    CNTLR_LOCK_RELEASE(timer_config_g.tmr_config_lock);
    return TIMER_FAILURE;
  }
  for(ll=0;ll<TIMER_MAX_NO_OF_CONTAINERS;ll++)
  {
    if(timer_config_g.container_head_p_p_p[ll][0] ==NULL)  /*Identifying the empty container*/
      break;
  }

  /*Increment the container count under lock*/
  timer_config_g.no_of_containers++;
  CNTLR_LOCK_RELEASE(timer_config_g.tmr_config_lock);
  for(ii=0;ii<CNTLR_NO_OF_THREADS_IN_SYSTEM + 1;ii++)/*1 extra for non worker threads*/
  {
    /* Allocate memory for Container */
    container_p =(struct tmr_container *)calloc(1,sizeof(struct tmr_container));
    if(container_p != NULL)
    {
      CNTLR_LOCK_INIT(container_p->tmr_container_lock);
      CNTLR_LOCK_INIT(tmr_container_lock);
      timer_config_g.container_head_p_p_p[ll][ii] = container_p;
      //printf("[%d] Address of pointer container_p=%p   %s %d\r\n",cntlr_thread_index,container_p,__FUNCTION__,__LINE__);
     
      /* Allocate memory for wheels to make it dynamic array */
      container_p->wheel_p = (struct tmr_wheel *) calloc(no_of_wheels,sizeof(struct tmr_wheel));
      if(container_p->wheel_p != NULL)
      {
        tmp_range = range;
        /*Compute number of buckets for each wheel */
        for(jj = 0; jj <= no_of_wheels-1; jj++)
        {
          if(tmp_range > 60*MICROSECS_PER_SEC)
          {
            if(granularity > 60*MICROSECS_PER_SEC)
              factor = granularity;
            else factor=60*MICROSECS_PER_SEC;
          }          
          else if(tmp_range > MICROSECS_PER_SEC)
          {
            if(granularity > MICROSECS_PER_SEC)
              factor = granularity;
            else factor=MICROSECS_PER_SEC;
          }      
          else if(tmp_range > MILLISECS_PER_SEC)
          {
            if(granularity > MILLISECS_PER_SEC)
              factor = granularity;
            else factor = MILLISECS_PER_SEC;
          }          
          else  /*if(j == uiNoOfWheels-1)*/
            factor = granularity;

          no_of_buckets = tmp_range/factor;

          #ifdef ROUNDED_OFF
          /*Round it off to 2 power x*/
          rounded_no_of_buckets = 1;
          while (rounded_no_of_buckets < no_of_buckets)
            rounded_no_of_buckets = (rounded_no_of_buckets << 1);
          #else 
          rounded_no_of_buckets = no_of_buckets;
          #endif
          #ifdef CNTLR_TIMER_DEBUG
          printf("TIMER  uiNoOfBuckets=%d  RoundedOff to =%d %s %d\r\n",no_of_buckets,rounded_no_of_buckets,__FUNCTION__,__LINE__);
          #endif

          /* Allocate memory for the buckets */
          container_p->wheel_p[jj].bucket_p = (struct tmr_bucket *)calloc(rounded_no_of_buckets,sizeof(struct tmr_bucket));

          if(container_p->wheel_p[jj].bucket_p != NULL)
          {
            container_p->wheel_p[jj].bucket_id=0;
            container_p->wheel_p[jj].size_of_wheel = rounded_no_of_buckets;
            container_p->wheel_p[jj].bucket_gap = factor; 
          }
          else
          {
            abort_b = TRUE;
            break;
          }
          tmp_range = factor;
        }
        if(abort_b == TRUE)
          break;
      } 

      /* Attach the container to the global list of containers head per thread.*/
      container_p->no_of_wheels = no_of_wheels;
      container_p->container_id_incont = ll;
      //container_p->next_p = NULL;
      #ifndef CONFIG_CNTLR_BOARD_X86
      GetJiffies(container_p->time_last_expiry);
      #else
      cntlr_get_time(&container_p->time_last_expiry);
      #endif
    }
  }
  do
  {
    /*Freeing up all allocated memory*/
    if(abort_b == FALSE)  //else /*Allocations are done fine so return the pcontainer pointer or the index ?*/
    {
      if(cntlr_ptr_ary_insert(&container_ptr_ary_g, &timer_config_g.container_head_p_p_p[ll][0], container_saferef) != TIMER_SUCCESS)
      {
        abort_b = TRUE;
        break;
      }
      if(tmr_add_to_rcu_list(ll) == TIMER_FAILURE)
      {
        cntlr_ptr_ary_remove_ex(&container_ptr_ary_g,container_saferef->index,container_saferef->magic);
        abort_b = TRUE;
        break;
      }
      /*Update the granularity, which will be used as time out for epoll*/
      for(ii=0;ii<CNTLR_NO_OF_THREADS_IN_SYSTEM + 1;ii++)/*1 extra for non worker threads*/
      {
        if(granularity < tmr_min_gran_g[ii])
          tmr_min_gran_g[ii] = granularity;
      }
      return TIMER_SUCCESS;  // safe reference 

    /* How should we handle if this function is invoked in runtime, 
    should update epoll if granularity of this container is less than current epoll */
    //  TODO Send an event to all the threads about the updated epoll value.
    //  return &TimerConfig_g.pContainerHead[l][0];
    // return l;
    //

    }
  }while(1);
  /*Control comes here only is there is some error condition*/ 
  for(ii=0;ii<CNTLR_NO_OF_THREADS_IN_SYSTEM +1 ;ii++)
  {
    if(timer_config_g.container_head_p_p_p[ll][ii] != NULL)
    {
      container_p = timer_config_g.container_head_p_p_p[ll][ii];
      for(jj=0;jj<container_p->no_of_wheels;jj++)
      {
        wheel_p = &container_p->wheel_p[jj];
        //if(container_p->wheel_p[j] != NULL)
        if(wheel_p != NULL)
        {
          for(kk=0;kk<wheel_p->size_of_wheel;kk++)
          {
            bucket_p = &wheel_p->bucket_p[kk];
            if(bucket_p != NULL)
              free(bucket_p);
          }
          free(wheel_p);
        }
      }
      free(container_p);
      timer_config_g.container_head_p_p_p[ll][ii]=NULL;
    }
  }

  /*Deccrement the container count under lock */
  CNTLR_LOCK_TAKE(timer_config_g.tmr_config_lock);
  timer_config_g.no_of_containers--;
  CNTLR_LOCK_RELEASE(timer_config_g.tmr_config_lock);
  printf("Timer container creation failed...  %s %d \r\n",__FUNCTION__,__LINE__);
  container_saferef = NULL;
  return TIMER_FAILURE;
}
/** \ingroup NONDPAATIMER

  <b>Description:</b>\n
  This function is used to create and start a timer. The function validates
  the container to which the timer is intended to add. It allocates memory
  for the timer node and puts it in the appropriate bucket. The timer node is 
  then added to the safe reference data structure and the index and magic
  number of the safe reference is then filled in the output parameter.

  <b>Input Parameters:</b>\n
  <i>pContainerSafeRef:</i> Pointer to safe reference of the container to which timer is to be added.
  <i>timeout_p:</i> Timeout of the timer in micro seconds.
  <i>Func_p:</i> Callback function which is to be called during the timer expiry.
  <i>args_p:</i> Arguments to the callback function.

  <b>Output Parameters</b>\n
  <i>pTmrNodeSafeRef:</i> Pointer to safe reference of the timer node that is created. 

  <b>Returns</b>\n
  <i> TIMER_SUCCESS </i> In case of successfull creation of Timer.
  <i> TIMER_FAILURE </i> In case of any error condition.
 */
/***************************************************************************/
/*
 * Function Name:timer_create_and_start
 *
 * Description: This API is used to create and start a timer. The caller should
 * provide the pointer to hold the timer node allocated in this api. This API
 * will put the timer node in the appropriate wheel and bucket.
 *
 * Parameters: 
 * pContainerSafeRef (I) Pointer to safe reference of the container to which timer is to be added.
 * pTmrNodeSafeRef (O) Pointer to safe reference of the timer node that is created. 
 * timeout_p (I) Timout value of the timer node.
 * Func_p (I) Function pointer which should be called when timer expires.
 * args_p (I) Arguments to the function pointer.
 *
 * Return values:
 * TIMER_FAILURE On error conditions.
 * pNode_p On successfull starting of timer.
 */
/***************************************************************************/
int32_t timer_create_and_start(struct saferef* container_saferef_p,struct tmr_saferef* tmr_saferef_p, 
                               uint8_t periodic_b,tmr_time_t time_out_p, tmr_cbk_func func_p,
                               void *arg1_p,void *arg2_p)
{
  struct   tmr_container** container_head_p_p,*container_p;
  struct   tmr_wheel* wheel_p;
  struct   tmr_bucket* tmp_bucket_p;
  int16_t  ii,ret_val;
  struct   tmr_node* node_p;

  uint8_t  heap_b = TRUE;
  uint8_t  wheel_identified_b;
  struct   tmr_rcu_cont_list *tmp_rcu_cont_list_p;
  //struct   tmr_rcu_cont_list  *derefer_container_p;
  uint64_t factor;

  /*Validate parameters*/
  if(func_p == NULL)
    return TIMER_ERROR_INVALID_CALLBACK_FUNCTION;

  if(container_saferef_p == NULL)
    container_head_p_p = timer_config_g.container_head_p_p_p[0];
  else
  {
    container_head_p_p = cntlr_ptr_ary_get_ptr_ex(&container_ptr_ary_g,(container_saferef_p)->index,(container_saferef_p)->magic);
    if(container_head_p_p == NULL)
    {
      printf("ThreadID=%d This container is no more valid  %s %d \r\n",cntlr_thread_index,__FUNCTION__,__LINE__);
      return TIMER_ERROR_INVALID_CONTAINER; 
    }
  }

  ret_val = mempool_get_mem_block(tmr_mempool_g,(uchar8_t **)&node_p,&heap_b);

  if(ret_val  != TIMER_SUCCESS)
  {
    printf("\r\n mempool allocation faile%dd\n",ret_val);
    return ret_val; 
  }
  (node_p)->arg1 = arg1_p;
  (node_p)->arg2 = arg2_p; 
  (node_p)->routine_p     = func_p;
  (node_p)->is_residual_b = FALSE;
  (node_p)->thread_id = cntlr_thread_index;
  (node_p)->heap_b    = heap_b;
  (node_p)->time_out  = time_out_p;

  container_p = container_head_p_p[cntlr_thread_index]; 
  CNTLR_RCU_READ_LOCK_TAKE();
  {
    /*Traverse the active list*/ 
    tmp_rcu_cont_list_p = rcu_dereference(rcu_cont_list_head_g);
    while(tmp_rcu_cont_list_p)
    {
      if(tmp_rcu_cont_list_p->container_id == container_p->container_id_incont)
        break;
      tmp_rcu_cont_list_p = rcu_dereference(tmp_rcu_cont_list_p->next_p);
    }

    if(tmp_rcu_cont_list_p == NULL)
    {
      CNTLR_RCU_READ_LOCK_RELEASE();
      mempool_release_mem_block(tmr_mempool_g, (uchar8_t*)node_p, node_p->heap_b);
      return TIMER_ERROR_INVALID_CONTAINER;
    }

    /*Now we identified the container, so dereference it for further use */

    //pDereferContainer=CNTLR_RCU_DEREFERENCE((void *) pTmpRcuContList);
    //derefer_container_p = tmp_rcu_cont_list_p;


    /*Based on ulTimeOut identify the Wheel and Bucket*/
    if(!cntlr_thread_index)
    CNTLR_LOCK_TAKE(container_p->tmr_container_lock);
    {
      /*Get current threads container */
      container_p = container_head_p_p[cntlr_thread_index]; 
      /*0 th ring is the lowest granularity ring or top most ring, i.e minutes or seconds,
       *  so starting to check from the highest granularity ring or bottom most ring i.e microseconds*/
      wheel_identified_b = 0;
      for(ii = container_p->no_of_wheels-1;ii>=0;ii--)  
      {
        /*if the timeout falls in the range of wheel, it means we should add the timer to this wheel*/
        if(time_out_p < (tmr_time_t)(container_p->wheel_p[ii].size_of_wheel * container_p->wheel_p[ii].bucket_gap))  
        {
          wheel_identified_b=1;
          break;
        }
      }

      if(cntlr_ptr_ary_insert(&tmr_node_ptr_ary_g[cntlr_thread_index],node_p,&(tmr_saferef_p->saferef)) != TIMER_SUCCESS)
      {
        if(!cntlr_thread_index)
          CNTLR_LOCK_RELEASE(container_p->tmr_container_lock);
        CNTLR_RCU_READ_LOCK_RELEASE();
        mempool_release_mem_block(tmr_mempool_g,(uchar8_t*)node_p,node_p->heap_b);
        return TIMER_FAILURE;
      }
      if(ii != container_p->no_of_wheels-1)
      {
        /*Add the time already processed by lower wheels*/
        //time_out_p += container_p->wheel_p[ii+1].bucket_id * container_p->wheel_p[ii+1].bucket_gap;
        time_out_p -= ((container_p->wheel_p[ii+1].size_of_wheel) - (container_p->wheel_p[ii+1].bucket_id)) * container_p->wheel_p[ii+1].bucket_gap;
      }
      if(wheel_identified_b == 0)
      {
        wheel_p = &container_p->wheel_p[0];
        if(time_out_p >(tmr_time_t) (wheel_p->size_of_wheel) *(wheel_p->bucket_gap)) 
          (node_p)->is_residual_b = TRUE;
        tmp_bucket_p = &wheel_p->bucket_p[wheel_p->size_of_wheel-1];
      }
      else
      {
        /*Now that we identified the wheel, we need to identify the bucket*/
        wheel_p = &container_p->wheel_p[ii];
        #ifdef ROUNDED_OFF
        tmp_bucket_p = &wheel_p->bucket_p[(wheel_p->bucket_id + ((time_out_p)/wheel_p->bucket_gap)-1) & (wheel_p->size_of_wheel -1)];
        #else
        /*When time out is less than granularity*/
        factor=(wheel_p->bucket_id +((time_out_p)/wheel_p->bucket_gap));
        tmp_bucket_p = &wheel_p->bucket_p[(factor) % (wheel_p->size_of_wheel)];
        #endif
      }
      (node_p)->next_p = tmp_bucket_p->head_p;
      if(tmp_bucket_p->head_p)
        tmp_bucket_p->head_p->prev_p=node_p;
      tmp_bucket_p->head_p=node_p;
      (node_p)->prev_p=NULL; 
      tmp_bucket_p->no_of_nodes++;
      container_p->no_of_tmr_nodes_incont ++; 
      if(container_p->no_of_tmr_nodes_incont ==1)
      {
        #ifndef CONFIG_CNTLR_BOARD_X86
        GetJiffies(container_p->time_last_expiry);
        #else
        cntlr_get_time(&container_p->time_last_expiry);
        #endif
      }
      (node_p)->bucket_p=(void *)tmp_bucket_p;  
      #ifdef ROUNDED_OFF
      printf("ThID=%d Added to Wheel=%d Bucket=%d Noofnodes =%d %u CurrentBucketID=%d\r\n",cntlr_tread_index_g,ii,((wheel_p->bucket_id + ((time_out_p)/wheel_p->bucket_gap)-1) & (wheel_p->size_of_wheel -1)),tmp_bucket_p->no_of_nodes,container_p->no_of_tmr_nodes_incont,wheel_p->bucket_id);
      #else
      //printf("ThID=%d Added to Wheel=%d Bucket=%d Noofnodes =%d %u CurrentBucketID=%d\r\n",of_uiTrnsprtThreadIndex_g, i,((wheel_p->bucket_id + ((ulTimeOut_p)/wheel_p->bucket_gap)-1) % (wheel_p->size_of_wheel)),tmp_bucket_p->no_of_nodes,container_p->no_of_tmr_nodes_incont,wheel_p->bucket_id);
      #endif
      node_p->index = tmr_saferef_p->saferef.index;
      node_p->magic = tmr_saferef_p->saferef.magic;
      node_p->periodic_b = periodic_b;
      //tmr_saferef_p->periodic_b = periodic_b;
      node_p->time_left = time_out_p;
      tmr_saferef_p->thread_id = cntlr_thread_index;

      /*If the timer is added with timeout greater than the range of the timer instance, such a node will be added
       *  to the last bucket of the top most ring. For such nodes residual should be set*/

      if(!cntlr_thread_index)
        CNTLR_LOCK_RELEASE(container_p->tmr_container_lock);
    }
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  /* Storing bucket pointer in node, This is required while adding and deleting timer nodes in the list. */
  return TIMER_SUCCESS;
}
/** \ingroup NONDPAATIMER

  <b>Description:</b>\n
  This function is used to stop the timer from getting expired. It involves
  removing the timer node from the bucket and freeing the memory, if this thread
  is the creator of the timer, else a job is created and queued to appropriate
  thread.

  <b>Input Parameters:</b>\n
  <i>pContainerSafeRef:</i> Pointer to safe reference of the container to which timer is added.
  <i>pTmrNodeSafeRef:</i> Pointer to safe reference of the timer node that is created. 

  <b>Output Parameters</b>\n

  <b>Returns</b>\n
  <i> TIMER_SUCCESS </i> In case of successfull creation of Timer.
  <i> TIMER_FAILURE </i> In case of any error condition.
 */
/***************************************************************************/
/*
 * Function Name:timer_stop_tmr
 *
 * Description: This API is used to stop a timer before its normal expiry happens.
 *
 * Parameters: 
 * pContainerSafeRef (I) void *, pointer to container.
 * node_p  (I) pspTmrNode_t *, Timer node.
 *
 * Return values:
 * TIMER_FAILURE on error condition
 * TIMER_SUCCESS on success.
 */
/***************************************************************************/
int32_t  timer_stop_tmr(struct saferef* container_saferef_p, struct tmr_saferef* tmr_node_saferef_p)
{
  struct  tmr_bucket* tmp_bucket_p;
  struct  tmr_container** container_head_p_p,*container_p;
  struct  tmr_node* node_p;
  struct  tmr_rcu_cont_list* tmp_rcu_cont_list_p;
  //struct  tmr_rcu_cont_list *derefer_container_p;
  int32_t ret_val;
  uint8_t heap_b = TRUE;
  
  if(tmr_node_saferef_p->saferef.magic == 0)
  {
    OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_WARN,"%s:%d, timer_stop_tmr: SAFE REFERENCE MAGIC value is ZERO...\r\n", __FUNCTION__, __LINE__); //Deepthi
    return TIMER_ERROR_INVALID_SAFEREF;
  }
  if(tmr_node_saferef_p->thread_id == cntlr_thread_index)
  {
    /* Validate input parameters */
    if(container_saferef_p == NULL)
      container_head_p_p = timer_config_g.container_head_p_p_p[0];
    else
    {
      container_head_p_p = cntlr_ptr_ary_get_ptr_ex(&container_ptr_ary_g,container_saferef_p->index,container_saferef_p->magic);
      if(container_head_p_p == NULL)
        return TIMER_ERROR_INVALID_CONTAINER; 
    }
    container_p = container_head_p_p[cntlr_thread_index]; 
    CNTLR_RCU_READ_LOCK_TAKE();
    {
      node_p = cntlr_ptr_ary_get_ptr_ex(&tmr_node_ptr_ary_g[cntlr_thread_index],tmr_node_saferef_p->saferef.index, tmr_node_saferef_p->saferef.magic);
      if(node_p == NULL)
      {
        CNTLR_RCU_READ_LOCK_RELEASE();
        return TIMER_FAILURE;
      }
      else if ((node_p->timer_expired_b) && (!node_p->periodic_b))  /*This node is currently being expired, so return success.*/
      {
        CNTLR_RCU_READ_LOCK_RELEASE();
        return TIMER_SUCCESS;
      }
      if(node_p->periodic_b)
        node_p->periodic_timer_stopped_b = 1;
      tmp_bucket_p = (struct tmr_bucket *)node_p->bucket_p;
      if(tmp_bucket_p->in_expiry_b) 
      {
        //printf("TIMER NODE Is in expiry list  %s %d\r\n",__FUNCTION__,__LINE__);
        /*This is to stop timer call back from getting invoked, when the timer is stopped 
         * from another timer callback which is in the same bucket.*/
        node_p->timer_stopped_b = 1;  
        CNTLR_RCU_READ_LOCK_RELEASE();
        return TIMER_SUCCESS;
      }
      /*Traverse the active list*/ 
      tmp_rcu_cont_list_p = rcu_dereference(rcu_cont_list_head_g);
      while(tmp_rcu_cont_list_p)
      {
        if(tmp_rcu_cont_list_p->container_id == container_p->container_id_incont)
          break;
        tmp_rcu_cont_list_p = rcu_dereference(tmp_rcu_cont_list_p->next_p);
      }
      if(tmp_rcu_cont_list_p == NULL)
      {
        printf("Container no more valid   %s %d\r\n",__FUNCTION__,__LINE__);
        CNTLR_RCU_READ_LOCK_RELEASE();
        return TIMER_FAILURE;
      }

      /*Now we identified the container, so dereference it for further use */
      //pDereferContainer=CNTLR_RCU_DEREFERENCE((void *) pTmpRcuContList);
      //derefer_container_p = tmp_rcu_cont_list_p;

        if(!cntlr_thread_index)
        CNTLR_LOCK_TAKE(container_p->tmr_container_lock);
        {
          if(node_p->prev_p == NULL)
          {
            //It is the first node of the list
            tmp_bucket_p->head_p=node_p->next_p;
            if(node_p->next_p)
              node_p->next_p->prev_p=NULL;
          }
          else
          {
            node_p->prev_p->next_p=node_p->next_p;
            if(node_p->next_p)
              node_p->next_p->prev_p=node_p->prev_p;
          }
          tmp_bucket_p->no_of_nodes--;
          container_p->no_of_tmr_nodes_incont --; 
          mempool_release_mem_block(tmr_mempool_g,
                                (uchar8_t*)node_p,
                                node_p->heap_b);
          cntlr_ptr_ary_remove_ex(&tmr_node_ptr_ary_g[cntlr_thread_index],tmr_node_saferef_p->saferef.index,tmr_node_saferef_p->saferef.magic);
          if(!cntlr_thread_index)
            CNTLR_LOCK_RELEASE(container_p->tmr_container_lock);
        }
    } 
    CNTLR_RCU_READ_LOCK_RELEASE();
    return TIMER_SUCCESS;
  } 
  else
  {
    /*Get a node from mempool and copy necessary data. Enqueue to the list of the target thread under lock.*/
    struct tmr_cont_and_node_saferef* tmr_stop_node_p;
    ret_val = mempool_get_mem_block(tmr_cont_and_node_saferef_mempool_g,(uchar8_t**)&tmr_stop_node_p,&heap_b);
    if(ret_val != TIMER_SUCCESS)
    {
      printf("MemPoolGetBlock Failed  %s %d\r\n",__FUNCTION__,__LINE__);
      return TIMER_FAILURE; 
    } 
    if(container_saferef_p == NULL)
    {
      tmr_stop_node_p->container_saferef.index = 0;
      tmr_stop_node_p->container_saferef.magic = 0;
    }
    else
    {
      tmr_stop_node_p->container_saferef.index = container_saferef_p->index;
      tmr_stop_node_p->container_saferef.magic = container_saferef_p->magic;
    }
    tmr_stop_node_p->tmr_node_saferef.saferef.index       = tmr_node_saferef_p->saferef.index;
    tmr_stop_node_p->tmr_node_saferef.saferef.magic       = tmr_node_saferef_p->saferef.magic;
    tmr_stop_node_p->tmr_node_saferef.thread_id   = tmr_node_saferef_p->thread_id;
    tmr_stop_node_p->next_p = NULL;
     
    tmr_enque_stop(tmr_stop_list_g + (tmr_node_saferef_p->thread_id),tmr_stop_node_p);
  }
  return TIMER_SUCCESS;
}
/***************************************************************************/
/** \ingroup NONDPAATIMER
  <b>Description:</b>\n
  This function is used to restart the timer from getting expired. It involves
  stopping the timer and creating a new timer with new timeout.
 
  <b>Input Parameters:</b>\n
  <i>pContainerSafeRef:</i> Pointer to safe reference of the container to which timer is added.
  <i>timeout_p:</i> Timeout of the timer in micro seconds.
  <i>Func_p:</i> Callback function which is to be called during the timer expiry.
  <i>args_p:</i> Arguments to the callback function.

  <b>Output Parameters</b>\n
  <i>pTmrNodeSafeRef:</i> Pointer to safe reference of the timer node that is created. 
 
  <b>Returns</b>\n
  <i> TIMER_SUCCESS </i> In case of successfull creation of Timer.
  <i> TIMER_FAILURE </i> In case of any error condition.
 */
/***************************************************************************/
/*
 * Function Name:timer_restart_tmr
 *
 * Description: This API is used to restart the timer with new timer parameters.
 *
 * Parameters: 
 * pContainer (I) pspTmrContainer_t *, Pointer to container.
 * node_p  (I) pspTmrNode_t *, The timer node.
 * timeOut (I) uint32_t, New time out with which the timer should be started.
 * Func  (I) pspTmrCbkFn, Timeout Call back function.
 * pArg  (I) void *, Arguments to timeout callback function.
 * Return values:. Avoid returning structure from functions so that exhaustive error type be supported.
 */
/***************************************************************************/
int32_t timer_restart_tmr(struct saferef* container_saferef_p, struct tmr_saferef* tmr_saferef_p, uint8_t periodic_b,
                          tmr_time_t time_out,tmr_cbk_func func, void* arg1_p,void* arg2_p)
{
  /* TODO Need to further optimise 
    1. Check if this is the parent thread if so no need to free the node to mempool, 
    2. just simpl remove the node from the bucket and fill appropriate fields and put the node in appropriate place again.*/
    timer_stop_tmr(container_saferef_p,tmr_saferef_p);
    return timer_create_and_start(container_saferef_p, tmr_saferef_p,periodic_b,time_out,func,arg1_p,arg2_p);
}
/***************************************************************************/
 /*
 * Function Name: redistribute_nodes
 *
 * Description:This function is used to redistribute the nodes from higher wheels to lower wheels.
 *
 * Parameters:
 * pContainer_p  (I) pspTmrcontainer_t *, Pointer to Container
 * wheel_index (I) uint16_t, Wheel Index
 * wheel_p  (I) pspTmrWheel_t *, The wheel which needs to be
 * Return values:. Avoid returning structure from functions so that exhaustive error type be supported.
 */
/***************************************************************************/
int32_t redistribute_nodes(struct tmr_container* container_p, uint16_t wheel_index, struct tmr_wheel* wheel_p)
{
  struct   tmr_wheel *next_wheel_p;
  //struct   tmr_wheel *wheel_tmp_p;
  struct   tmr_node *tmr_list_p,*tmp_node_p;
  struct   tmr_bucket* tmp_bucket_p;
  uint32_t bucket_id,ii;
  uint64_t factor;

  next_wheel_p = &container_p->wheel_p[wheel_index-1];  /*Wheel from where nodes should be removed*/
  tmr_list_p = next_wheel_p->bucket_p[next_wheel_p->bucket_id].head_p;

  next_wheel_p->bucket_p[next_wheel_p->bucket_id].head_p = NULL;
  /*If the next wheel is the top most, and its bucket id is at the end, then we need to see if the timer nodes are residual*/
  if((wheel_index == 1) && (next_wheel_p->bucket_id == next_wheel_p->size_of_wheel-1))
  {
    while(tmr_list_p)
    {
      tmp_node_p = tmr_list_p;
      tmp_node_p->time_left -= (next_wheel_p->bucket_gap * next_wheel_p->size_of_wheel);
                                tmr_list_p=tmr_list_p->next_p;
      if(tmp_node_p->time_left >= (tmr_time_t)(next_wheel_p->bucket_gap * next_wheel_p->size_of_wheel))
      {
        tmp_bucket_p = &next_wheel_p->bucket_p[next_wheel_p->bucket_id];
        if(tmp_bucket_p->head_p)
          tmp_bucket_p->head_p->prev_p = tmp_node_p;
        tmp_node_p->next_p = tmp_bucket_p->head_p;
        tmp_bucket_p->head_p = tmp_node_p;
        tmp_node_p->bucket_p = tmp_bucket_p;
        tmp_node_p->prev_p   = NULL;
        /*Since we are adding to the same bucket the no_of_nodes is not updated*/
      }
      else /*redistribute put it in appropriate wheel and bucket */
      {
        for(ii = container_p->no_of_wheels-1;ii>=0; ii--)
        {
          /*if the timeout falls in the range of wheel, it means we should add the timer to this wheel*/
          if(tmp_node_p->time_left < (tmr_time_t)(container_p->wheel_p[ii].size_of_wheel * container_p->wheel_p[ii].bucket_gap))
            break;
        }
        //wheel_tmp_p = &container_p->wheel_p[ii];
        /*When time out is less than granularity*/
        factor = (tmp_node_p->time_left)/(tmr_time_t)wheel_p->bucket_gap;
        tmp_bucket_p = &wheel_p->bucket_p[(wheel_p->bucket_id + factor) % (tmr_time_t)(wheel_p->size_of_wheel)];
        (tmp_node_p)->next_p=tmp_bucket_p->head_p;
        
        if(tmp_bucket_p->head_p)
        {
          tmp_bucket_p->head_p->prev_p = tmp_node_p;
        }
        tmp_bucket_p->head_p=tmp_node_p;
        (tmp_node_p)->is_residual_b = FALSE;
        (tmp_node_p)->prev_p=NULL;
        (tmp_node_p)->bucket_p=tmp_bucket_p;
        tmp_bucket_p->no_of_nodes++;
        next_wheel_p->bucket_p[next_wheel_p->bucket_id].no_of_nodes--;
      }
    }
  }
  else
  {
    while(/*tmr_list_p && */ next_wheel_p->bucket_p[next_wheel_p->bucket_id].no_of_nodes )
    {
      //TODO Compute the bucket id . Since this happens upon reaching the end of lowest wheel, % operator is used. 
      // Scope for effective logic ?
      tmr_list_p->time_left=tmr_list_p->time_left%((tmr_time_t)wheel_p->bucket_gap* wheel_p->size_of_wheel);
      bucket_id=tmr_list_p->time_left/(tmr_time_t)wheel_p->bucket_gap;

      /* Insert into this bucket */
      tmp_bucket_p=&wheel_p->bucket_p[bucket_id];
      tmp_node_p=tmr_list_p->next_p;

      tmr_list_p->next_p=tmp_bucket_p->head_p;
      if(tmp_bucket_p->head_p)
      {
        tmp_bucket_p->head_p->prev_p=tmr_list_p;
      }
      tmr_list_p->bucket_p=tmp_bucket_p;
      tmr_list_p->prev_p=NULL;
      tmp_bucket_p->head_p=tmr_list_p;

      /*Increment the counter*/
      tmp_bucket_p->no_of_nodes++;
      next_wheel_p->bucket_p[next_wheel_p->bucket_id].no_of_nodes--;
      tmr_list_p=tmp_node_p;
    }
  }

  if(next_wheel_p->bucket_id < next_wheel_p->size_of_wheel -1)
  {
    next_wheel_p->bucket_id++;
    return REDISTRIBUTION_NOT_REQUIRED;
  }
  else
  {
    /*ReDristribution is required from the next wheel also */
    next_wheel_p->bucket_id=0;
    return REDISTRIBUTION_REQUIRED;
  }
}
/***************************************************************************/
/*
 * Function Name: timer_process_timers
 *
 * Description: This is the core API which performs timer nodes expiry.
 * Also it performs redistribution of nodes.
 
 * Parameters: void
 * Return values: void
 */
/***************************************************************************/
void timer_process_timers(void)
{
  struct  tmr_container* container_p;
  struct  tmr_wheel* wheel_p;
  struct  tmr_bucket* bucket_p;
  struct  tmr_node* node_p;
  int32_t ii,ret_val;
  double  diff;

#ifndef CONFIG_CNTLR_BOARD_X86
        uint64_t current_jiffies;
#else
        uint64_t current_time;
#endif

  struct tmr_rcu_cont_list *tmp_rcu_cont_list_p,*derefer_container_p;
  CNTLR_RCU_READ_LOCK_TAKE();
  {
    /* Check for any timer stop nodes present in the list, if so process them.*/
    if(tmr_stop_list_g[cntlr_thread_index].tmr_stop_count)
      tmr_deque_stop(tmr_stop_list_g + cntlr_thread_index);
    tmp_rcu_cont_list_p = rcu_dereference(rcu_cont_list_head_g);

    while(tmp_rcu_cont_list_p)
    {
      derefer_container_p = rcu_dereference((void *)tmp_rcu_cont_list_p);
      container_p = timer_config_g.container_head_p_p_p[derefer_container_p->container_id][cntlr_thread_index];
      if(cntlr_thread_index)
      {
        if(container_p->no_of_tmr_nodes_incont > 0)
        {
          #ifndef CONFIG_CNTLR_BOARD_X86
          GetJiffies(current_jiffies);
          diff=((current_jiffies - container_p->time_last_expiry) * (unsigned long long)1000) / TIMER_TICKS_PER_SEC;
          #else
          cntlr_get_time(&current_time);
          diff = (current_time - container_p->time_last_expiry)/**1000*/;
          #endif
          while(diff > ((container_p->wheel_p[container_p->no_of_wheels-1].bucket_gap)/1000)*0.9)
          {  
             wheel_p=&container_p->wheel_p[container_p->no_of_wheels-1];
             bucket_p=&wheel_p->bucket_p[wheel_p->bucket_id];
             bucket_p->in_expiry_b = TRUE;

             while(bucket_p->no_of_nodes)
             {
               /*Traverse through the nodes and call the expir functions with arguments*/
               node_p = bucket_p->head_p;
               /* Make the head pointer point to next timer node */
               bucket_p->head_p = node_p->next_p;
               bucket_p->no_of_nodes--;
               
               node_p->timer_expired_b = 1;

               /* Execute the callback function*/
               if(!node_p->timer_stopped_b)
                 node_p->routine_p(node_p->arg1,node_p->arg2);
               
               /*If it is a periodic timer, put it in appropriate wheel n bucket but dont delete the node*/
               if((node_p->periodic_b)&&(!node_p->periodic_timer_stopped_b))
               {
                 /*Identify the wheel based on timeout and insert in bucket.*/
                 //printf("\r\n periodic timer 1 \n");
                 tmr_insert_periodic_tmr(node_p,container_p);
               }
               else
               {
                 /* Remove from safe ref*/
                 cntlr_ptr_ary_remove_ex(&tmr_node_ptr_ary_g[cntlr_thread_index],node_p->index,node_p->magic);
                 /* Delete the node*/
                 mempool_release_mem_block(tmr_mempool_g,(uchar8_t*)node_p,node_p->heap_b);
                 container_p->no_of_tmr_nodes_incont--;
               }
               /*  Decrement the count of nodes. */
               //bucket_p->no_of_nodes--;
             }
             bucket_p->in_expiry_b = FALSE;
             /* Check if we reached the end of the bucket , if  not increment the bucket id to point to next bucket in that wheel*/
             if(wheel_p->bucket_id < wheel_p->size_of_wheel-1)
             {
               wheel_p->bucket_id++;
               //printf("BucketID=%d   %s %d \r\n", wheel_p->bucket_id,__FUNCTION__,__LINE__);
             }
             else /*The current wheel reached its range, i.e wheel_p->bucket_id == wheel_p->size_of_wheel-1*/
             {
               #ifdef CNTLR_TIMER_DEBUG
               printf("BucketID=%d reached its end hence cascading  %s %d \r\n", wheel_p->bucket_id,__FUNCTION__,__LINE__);
               #endif
               /*Move the bucket pointer to zero, so that next time you process zero bucket*/
               wheel_p->bucket_id=0;

              /*Cascade*/
              for(ii = container_p->no_of_wheels-1;ii;ii--)
              {
                wheel_p = &container_p->wheel_p[ii];
                ret_val = redistribute_nodes(container_p,ii,wheel_p);
                if(ret_val == REDISTRIBUTION_NOT_REQUIRED)
                break;
              }
            }
            diff -= (container_p->wheel_p[container_p->no_of_wheels-1].bucket_gap)/1000;
            #ifndef CONFIG_CNTLR_BOARD_X86
            GetJiffies(container_p->time_last_expiry);
            #else
            cntlr_get_time(&container_p->time_last_expiry);
            #endif
         }
      }
      else
      {
        #ifndef CONFIG_CNTLR_BOARD_X86
        GetJiffies(container_p->time_last_expiry);
        #else
        cntlr_get_time(&container_p->time_last_expiry);
        #endif
      }
    }
    else  /*Non Worker Thread*/
    {
      CNTLR_LOCK_TAKE(container_p->tmr_container_lock);
      if(container_p->no_of_tmr_nodes_incont > 0)
      {
        #ifndef CONFIG_CNTLR_BOARD_X86
        GetJiffies(current_jiffies);
        diff = ((current_jiffies- container_p->time_last_expiry) * (unsigned long long)1000) / TIMER_TICKS_PER_SEC;
        #else
        cntlr_get_time(&current_time);
        diff=(current_time - container_p->time_last_expiry)/**1000*/;
        #endif
       
        while(diff > ((container_p->wheel_p[container_p->no_of_wheels-1].bucket_gap)/1000)*0.9)
        {
          wheel_p = &container_p->wheel_p[container_p->no_of_wheels-1];
          bucket_p = &wheel_p->bucket_p[wheel_p->bucket_id];
          bucket_p->in_expiry_b = TRUE;
          
          while(bucket_p->no_of_nodes)
          {
            /*Traverse through the nodes and call the expir functions with arguments*/
            node_p = bucket_p->head_p;
            /* Make the head pointer point to next timer node */
            bucket_p->head_p = node_p->next_p;
            bucket_p->no_of_nodes--;

            node_p->timer_expired_b = 1;

            /* Execute the callback function*/
            CNTLR_LOCK_RELEASE(container_p->tmr_container_lock);
            if(!node_p->timer_stopped_b)
            {
              //printf("\r\n calling the callback function\n");
              node_p->routine_p(node_p->arg1,node_p->arg2);
            }
            CNTLR_LOCK_TAKE(container_p->tmr_container_lock);

            /* Make the head pointer point to next timer node */
            //bucket_p->head_p=node_p->next_p;
            /*If it is a periodic timer, put it in appropriate wheel n bucket but dont delete the node*/
            if((node_p->periodic_b)&&(!node_p->periodic_timer_stopped_b))
            {
              /*Identify the wheel based on timeout and insert in bucket.*/
              //printf("\r\n periodic timer 2 \n");
              tmr_insert_periodic_tmr(node_p,container_p);
            }
            else
            {
              /* Remove from safe ref*/
              cntlr_ptr_ary_remove_ex(&tmr_node_ptr_ary_g[cntlr_thread_index],node_p->index,node_p->magic);
              /* Delete the node*/
              mempool_release_mem_block(tmr_mempool_g,(uchar8_t*)node_p,node_p->heap_b);
              container_p->no_of_tmr_nodes_incont --;
            }
          }
          bucket_p->in_expiry_b = FALSE;
          /* Check if we reached the end of the bucket , if  not increment the bucket id to point to next bucket in that wheel*/
          if(wheel_p->bucket_id < wheel_p->size_of_wheel-1)
            wheel_p->bucket_id++;
          else /*The current wheel reached its range, i.e wheel_p->bucket_id == wheel_p->size_of_wheel-1*/
          {
            #ifdef CNTLR_TIMER_DEBUG
            printf("BucketID=%d reached its end hence cascading  %s %d \r\n", wheel_p->bucket_id,__FUNCTION__,__LINE__);
            #endif
            /*Move the bucket pointer to zero, so that next time you process zero bucket*/
            wheel_p->bucket_id = 0;
            /*Cascade*/
            for(ii = container_p->no_of_wheels-1;ii;ii--)
            {
              wheel_p = &container_p->wheel_p[ii];
              ret_val = redistribute_nodes(container_p,ii,wheel_p);
              if(ret_val == REDISTRIBUTION_NOT_REQUIRED)
                break;
            }
          }
          diff -= (container_p->wheel_p[container_p->no_of_wheels-1].bucket_gap)/1000;

          #ifndef CONFIG_CNTLR_BOARD_X86
          GetJiffies(container_p->time_last_expiry);
          #else
          cntlr_get_time(&container_p->time_last_expiry);
          #endif
        }
      }
      else
      {
        #ifndef CONFIG_CNTLR_BOARD_X86
        GetJiffies(container_p->time_last_expiry);
        #else
        cntlr_get_time(&container_p->time_last_expiry);
        #endif
      }
      CNTLR_LOCK_RELEASE(container_p->tmr_container_lock);
    }
    tmp_rcu_cont_list_p = rcu_dereference(tmp_rcu_cont_list_p->next_p);
  }
}
CNTLR_RCU_READ_LOCK_RELEASE();
}
/** \ingroup NONDPAATIMER

 <b>Description:</b>\n
 This function is used to delete the container.

 <b>Input Parameters:</b>\n
 <i>pContainerSafeRef:</i> Pointer to safe reference of the container to be deleted.
 <i>bFalg:</i> Flag to determine  whether timer nodes if present in the container should be stopped or not.
 <b>Output Parameters</b>\n
 
 <b>Returns</b>\n
 <i> TIMER_SUCCESS </i> In case of successfull creation of Timer.
 <i> TIMER_FAILURE </i> In case of any error condition.
 */
/***************************************************************************/
/*
 * Function Name: timer_delete_default_container
 *
 * Description: This function is used to delete a container.
 * Parameters: 
 * Return values:. Avoid returning structure from functions so that exhaustive error type be supported.
 */
/***************************************************************************/
int32_t timer_delete_default_container(struct saferef* container_saferef_p ,uint8_t flag_b)
{
  struct tmr_container*  container_p;
  struct tmr_container** container_head_p_p;

  container_head_p_p = cntlr_ptr_ary_get_ptr_ex(&container_ptr_ary_g,container_saferef_p->index,container_saferef_p->magic);

  if(container_head_p_p == NULL)
    return TIMER_ERROR_INVALID_CONTAINER;

  container_p = container_head_p_p[0];

  if(tmr_del_from_rcu_list(container_p,flag_b) == TIMER_FAILURE)
    return TIMER_ERROR_FAILED_TO_DELETE_CONTAINER;

   cntlr_ptr_ary_remove_ex(&container_ptr_ary_g,container_saferef_p->index,container_saferef_p->magic);
   return TIMER_SUCCESS;
   // TODO: Update pTmrMinGran_g  HOW ? 
   // Lowest Wheel, inter bucket gap will give the granularity of contianer.
}
/***************************************************************************/
/*
 * Function Name: timer_module_deinit
 *
 * Description: This function should be called during module deinit. This function will
 * delete default container, and frees other memory allocated during init time.
 *
 * Parameters: T_VOID
 * Return values: T_VOID
 */
/***************************************************************************/
void timer_module_deinit(void)
{
  int16_t ii;
  if(timer_delete_default_container(&def_cont_saferef,1/*TRUE*/) == TIMER_SUCCESS)
    printf("Default Container Deletion Successfull %s %d\r\n",__FUNCTION__,__LINE__);
  else
    printf("Default Container Deletion failed %s %d\r\n",__FUNCTION__,__LINE__);

  for(ii=0;ii< TIMER_MAX_NO_OF_CONTAINERS;ii++)
    free(timer_config_g.container_head_p_p_p[ii]);

  free(tmr_min_gran_g);
  for(ii=0;ii<CNTLR_NO_OF_THREADS_IN_SYSTEM +1 ;ii++)
    cntlr_ptr_ary_deinit(&tmr_node_ptr_ary_g[ii]);
  free(tmr_node_ptr_ary_g);
  free(timer_config_g.container_head_p_p_p);
  cntlr_ptr_ary_deinit(&container_ptr_ary_g);
  mempool_delete_pool(tmr_mempool_g);
}
/*************************************************************************/
/*
 * function name: timer_get_least_granularity
 * description: this api is used by main before going into pool to get the 
 * least granularity of containers.
 * parameters: none
 * return values: returns minimum granularity of the thread.
 */
/***************************************************************************/
uint32_t timer_get_least_granularity(void)
{
  // Core module expects only in milli seconds.
  return __timer_get_least_granularity();
}
/***************************************************************************/
/*
 * function name: Timer_get_tmr_info
 *
 * description: this api is used by menu, to print the timer info like per
 * thread number of timers in each container.
 *
 * parameters: none
 * return values: none.
 */
/***************************************************************************/
void timer_get_tmr_info(void)
{
  int16_t ii,jj;

  for(ii=0; timer_config_g.container_head_p_p_p[ii][0]!=NULL ; ii++)
  {
    for(jj=0; jj<CNTLR_NO_OF_THREADS_IN_SYSTEM+1; jj++)
    printf("No of timers in thread %d is %u and stop timer count=%u \r\n",jj,
    timer_config_g.container_head_p_p_p[ii][jj]->no_of_tmr_nodes_incont,tmr_stop_list_g[ii].tmr_stop_count);
  }
}

void tmr_enque_stop(struct tmr_stop_list* tmr_list_p, struct tmr_cont_and_node_saferef* tmr_stop_node_p)
{
  CNTLR_LOCK_TAKE(tmr_list_p->tmr_stop_list_mutex);
  if(tmr_list_p->head_p == NULL)
    tmr_list_p->head_p = tmr_stop_node_p;
  else
    tmr_list_p->tail_p->next_p = tmr_stop_node_p;

  tmr_list_p->tail_p = tmr_stop_node_p;
  tmr_list_p->tmr_stop_count++;
  CNTLR_LOCK_RELEASE(tmr_list_p->tmr_stop_list_mutex);
}

void tmr_deque_stop(struct tmr_stop_list* tmr_list_p)
{
  struct tmr_cont_and_node_saferef* tmr_stop_node_p,*temp_p;
  CNTLR_LOCK_TAKE(tmr_list_p->tmr_stop_list_mutex);
  temp_p = tmr_list_p->head_p;
  tmr_list_p->head_p  = NULL;
  tmr_list_p->tail_p  = NULL;
  tmr_list_p->tmr_stop_count = 0;
  CNTLR_LOCK_RELEASE(tmr_list_p->tmr_stop_list_mutex);
  while(temp_p)
  {
    tmr_stop_node_p = temp_p;
    if(tmr_stop_node_p->container_saferef.index == 0 && tmr_stop_node_p->container_saferef.magic == 0)
    {
      if(timer_stop_tmr(NULL, &(tmr_stop_node_p->tmr_node_saferef)) == TIMER_FAILURE)
      {
        OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_WARN,"%s:%d, Timer Stop Failed...", __FUNCTION__, __LINE__);  //Deepthi
        printf("Timer Stop Failed.. %s %d\r\n",__FUNCTION__,__LINE__);
      }
    }
    else
    {
      if(timer_stop_tmr(&(tmr_stop_node_p->container_saferef), &(tmr_stop_node_p->tmr_node_saferef)) == TIMER_FAILURE)
        printf("Timer Stop Failed.. %s %d\r\n",__FUNCTION__,__LINE__);
    }
    temp_p=tmr_stop_node_p->next_p;
    mempool_release_mem_block(tmr_cont_and_node_saferef_mempool_g,(uchar8_t*)tmr_stop_node_p,tmr_stop_node_p->heap_b);
  }
}
/*****************************************************************************/
void tmr_insert_periodic_tmr(struct tmr_node* node_p,struct tmr_container* container_p)
{
  struct   tmr_wheel*  wheel_p;
  struct   tmr_bucket* tmp_bucket_p;
  uint64_t time_out,factor;
  uint32_t ii;

  //if(!cntlr_thread_index)   
    //CNTLR_LOCK_TAKE(container_p->tmr_container_lock);

  time_out = node_p->time_out;
  /* 0th ring is the lowest granularity ring or top most ring, i.e minutes or seconds,
     so starting to check from the highest granularity ring or bottom most ring i.e microseconds*/
  for(ii = container_p->no_of_wheels-1; ii>=0; ii--)
  {
    /*if the timeout falls in the range of wheel, it means we should add the timer to this wheel*/
    if(time_out < (container_p->wheel_p[ii].size_of_wheel * container_p->wheel_p[ii].bucket_gap))
      break;
  }
  /*Now that we identified the wheel, we need to identify the bucket*/
  wheel_p = &container_p->wheel_p[ii];
#ifdef ROUNDED_OFF
  tmp_bucket_p = &wheel_p->bucket_p[(wheel_p->bucket_id + ((time_out)/wheel_p->bucket_gap)-1) & (wheel_p->size_of_wheel -1)];
#else
  /*When time out is less than granularity*/
  //factor=(time_out)/wheel_p->bucket_gap;
  factor=(wheel_p->bucket_id +((time_out)/wheel_p->bucket_gap));  
#if 0
  if(time_out < container_p->wheel_p[container_p->no_of_wheels-1].bucket_gap)
  {
    /*We cannot allow to create a timer in the current bucket. 
       So adding it to next bucket */
    tmp_bucket_p = &wheel_p->bucket_p[(wheel_p->bucket_id + factor+1) % (wheel_p->size_of_wheel)];
  }
  else
    tmp_bucket_p=&wheel_p->bucket_p[(wheel_p->bucket_id + factor) % (wheel_p->size_of_wheel)];
#endif
  tmp_bucket_p = &wheel_p->bucket_p[(factor) % (wheel_p->size_of_wheel)];
#endif
    (node_p)->next_p = tmp_bucket_p->head_p;
    if(tmp_bucket_p->head_p)
      tmp_bucket_p->head_p->prev_p = node_p;
    tmp_bucket_p->head_p = node_p;
    (node_p)->prev_p=NULL;
    tmp_bucket_p->no_of_nodes++;
    if(container_p->no_of_tmr_nodes_incont ==1)
    {
#ifndef CONFIG_CNTLR_BOARD_X86
      GetJiffies(container_p->time_last_expiry);
#else
      cntlr_get_time(&container_p->time_last_expiry);
#endif
    }
     (node_p)->bucket_p = (void *) tmp_bucket_p;
#ifdef ROUNDED_OFF
     printf("ThID=%d Added to Wheel=%d Bucket=%d Noofnodes =%d %u CurrentBucketID=%d\r\n",cntlr_thread_index, ii,
            ((wheel_p->bucket_id + ((time_out)/wheel_p->bucket_gap)-1) & (wheel_p->size_of_wheel -1)),
            tmp_bucket_p->no_of_nodes,container_p->no_of_tmr_nodes_incont,wheel_p->bucket_id);
#else
//  printf("ThID=%d Added to Wheel=%d Bucket=%d Noofnodes =%d %u CurrentBucketID=%d\r\n",of_uiTrnsprtThreadIndex_g, i,((wheel_p->bucket_id + ((time_out)/wheel_p->bucket_gap)-1) % (wheel_p->size_of_wheel)),pTmpBucket->no_of_nodes,container_p->no_of_tmr_nodes_incont,wheel_p->bucket_id);
 #endif
 if(ii != container_p->no_of_wheels-1)
 {
   /*Add the time already processed by lower wheels*/
   time_out += container_p->wheel_p[ii+1].bucket_id * container_p->wheel_p[ii+1].bucket_gap;
 }
 node_p->timer_expired_b = 1;
 node_p->time_left = time_out;
 //if(!cntlr_thread_index)
   //CNTLR_LOCK_RELEASE(container_p->tmr_container_lock);
}
/**********************************************************************************/
/*
 * Function Name: tmr_add_to_rcu_list
 *
 * Description:This function is used to add a container to the rcu protected
 * linked list in which each node holds the container id.
 *
 * Parameters:uiContainerID  (I) ID of the container that is created.
 *
 * Return values: TIMER_FAILURE is case of any error condition
 * else TIMER_SUCCESS
 */
/********************************************************************************/
int32_t tmr_add_to_rcu_list(int32_t container_id)
{
  struct tmr_rcu_cont_list* node_p,*temp_p;

  node_p = (struct tmr_rcu_cont_list *)calloc(1,sizeof(struct tmr_rcu_cont_list));
  if(node_p == NULL)
  {
    printf("Memory allocation error");
    return TIMER_FAILURE;
  }
  node_p->container_id = container_id;
  CNTLR_LOCK_TAKE(rcu_cont_list_lock_g);
  if(rcu_cont_list_head_g == NULL)
  {
    CNTLR_RCU_ASSIGN_POINTER(rcu_cont_list_head_g,node_p);
    CNTLR_LOCK_RELEASE(rcu_cont_list_lock_g);
    return TIMER_SUCCESS;
  }
  temp_p = rcu_cont_list_head_g;
  while (temp_p->next_p != NULL)
    temp_p = temp_p->next_p;
  rcu_assign_pointer(temp_p->next_p,node_p);
  rcu_assign_pointer(node_p->prev_p,temp_p);
  CNTLR_LOCK_RELEASE(rcu_cont_list_lock_g);
  return TIMER_SUCCESS;
}
/*********************************************************************************/
/*
 * Function Name: tmr_del_from_rcu_list
 * Description:This function is used to delete a container from the rcu protected
 * linked list in which each node holds the container id.
 
 * Parameters:pDelContainer  (I) Pointer to container.
 *   bFlag  (I) Flag to forcefully delete all nodes in the container.
 *
 * Return values: TIMER_FAILURE is case of any error condition
 * else TIMER_SUCCESS
 */
/********************************************************************************/
int32_t tmr_del_from_rcu_list(struct tmr_container* del_container_p,uint8_t flag_b)
{
  /* Take RCU Write lock, traverse through the list, remove the node.*/
  struct tmr_rcu_cont_list *temp_cont_p;
  struct tmr_container* container_p;
  uint16_t ii;
  
  CNTLR_LOCK_TAKE(rcu_cont_list_lock_g);
  temp_cont_p = rcu_cont_list_head_g;

  while(temp_cont_p)
  {
    if(temp_cont_p->container_id == del_container_p->container_id_incont)
      break;
    temp_cont_p = temp_cont_p->next_p;
  }

  if(temp_cont_p == NULL)
  {
    printf("ERROR.... No matching container id found  %s %d\r\n",__FUNCTION__,__LINE__);
    CNTLR_LOCK_RELEASE(rcu_cont_list_lock_g);
    return TIMER_FAILURE;
  }
  /*Now we identified the container, 
   * If bFlag is TRUE means forcefully delete the nodes, if FALSE means return FAILURE when container has nodes across all threads*/
  if(temp_cont_p->prev_p)
    rcu_assign_pointer(temp_cont_p->prev_p->next_p,temp_cont_p->next_p);  
  if(temp_cont_p->next_p)
    rcu_assign_pointer(temp_cont_p->next_p->prev_p,temp_cont_p->prev_p);

  if(flag_b == FALSE)
  {
   for(ii=0;ii<CNTLR_NO_OF_THREADS_IN_SYSTEM+1;ii++)
   {
     container_p = timer_config_g.container_head_p_p_p[del_container_p->container_id_incont][ii];
     if(container_p->no_of_tmr_nodes_incont)
     {
       if(temp_cont_p->prev_p)
         rcu_assign_pointer(temp_cont_p->prev_p->next_p,temp_cont_p);
       if(temp_cont_p->next_p)
         rcu_assign_pointer(temp_cont_p->next_p->prev_p,temp_cont_p);
       /* TODO Reassign the RCU lock, to previous node*/
       // synchronize_rcu();
       CNTLR_LOCK_RELEASE(rcu_cont_list_lock_g);
       return TIMER_FAILURE;
     }
   }
   /*Code reaches here only if no timer nodes are present across all threads.*/
   {
     /*TODO use call_rcu*/
     tmr_delete_container_with_id(del_container_p->container_id_incont);
   }
 }
 else
 {
   /*TODO use call_rcu*/
  tmr_delete_container_with_id(del_container_p->container_id_incont);
 }
 //synchronize_rcu();
 CNTLR_LOCK_RELEASE(rcu_cont_list_lock_g);
 free(temp_cont_p);
 return TIMER_SUCCESS;
}
/****************************************************************************/
/*
 * Function Name: tmr_delete_container_with_id 
 *
 * Description:This function is used to delete the actual container
 * from all the threads context.
 *
 * Parameters:container_id (I) Container ID.
 *
 * Return values: NONE
 */
/***************************************************************************/
void tmr_delete_container_with_id(uint32_t container_id)
{
  uint32_t ii,jj,kk;
  struct tmr_wheel     *wheel_p;
  struct tmr_bucket    *bucket_p;
  struct tmr_node      *node_p;
  struct tmr_container *container_p;

  for(ii=0;ii<CNTLR_NO_OF_THREADS_IN_SYSTEM +1 ;ii++)
  {
    if(timer_config_g.container_head_p_p_p[container_id][ii] != NULL)
    {
      container_p = timer_config_g.container_head_p_p_p[container_id][ii];
      for(jj=0;jj<container_p->no_of_wheels;jj++)
      {
        wheel_p = &container_p->wheel_p[jj];
        if(wheel_p != NULL)
        {
          for(kk=0;kk<wheel_p->size_of_wheel;kk++)
          {
            bucket_p = &wheel_p->bucket_p[kk];
            if(bucket_p != NULL)
            {
              while(bucket_p->no_of_nodes)
              {
                #ifdef CNTLR_TIMER_DEBUG
                printf("TIMER Noofnodes=%u BucketId=%u %s %d \r\n",bucket_p->no_of_nodes,wheel_p->bucket_id,__FUNCTION__,__LINE__);
                #endif
                /*Traverse through the nodes and call the expir functions with arguments*/
                node_p = bucket_p->head_p;
                /* Make the head pointer point to next timer node */
                bucket_p->head_p = node_p->next_p;
                /* Delete the node*/
                mempool_release_mem_block(tmr_mempool_g,(uchar8_t *)node_p,node_p->heap_b);
                /*  Decrement the count of nodes. */
                bucket_p->no_of_nodes--;
                container_p->no_of_tmr_nodes_incont--;
              }
            }
          }
          free(wheel_p->bucket_p);
        }
      }
      free(container_p->wheel_p);
      free(container_p);
      timer_config_g.container_head_p_p_p[container_id][ii]=NULL;
    }
  }
  return;
}
/***************************************************************************/  
/*
 * Function Nametmr:tmr_num_of_wheels
 *
 * Description:This function is used to compute the number of wheels given the 
 * granularity and range while creating the container. 
 *
 * Parameters: Granularity (I) Granularity of the timer container.
 *   Range (I) Range of the timer container.
 *
 * Return values:. Returns the number of wheels required based on granularity 
 * and range.
 */
/***************************************************************************/
uint32_t tmr_num_of_wheels(uint32_t granularity, uint32_t range)
{
  uint32_t no_of_wheels=0;

  if(range > 60*MICROSECS_PER_SEC)
    no_of_wheels=4;
  
  else if(range > MICROSECS_PER_SEC)
    no_of_wheels=3;
  
  else if(range > MILLISECS_PER_SEC)
    no_of_wheels=2;
  
  else if(range > 10)
    no_of_wheels=1;
  
  if(granularity >= MILLISECS_PER_SEC && granularity < MICROSECS_PER_SEC)
    no_of_wheels --;
  
  else if(granularity >= MICROSECS_PER_SEC && granularity < 60*MICROSECS_PER_SEC)
    no_of_wheels = no_of_wheels-2;
  printf("\r\n Granularity = %d\n",granularity); 
  return no_of_wheels;
}
/***************************************************************************/
