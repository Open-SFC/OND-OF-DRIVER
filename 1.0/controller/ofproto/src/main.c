/**
 * Copyright (c) 2012, 2013  Freescale.
 *  
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

/* File  :      main.c
 * Author:      Rajesh Madabushi <rajesh.madabushi@freescale.com>
 * Description: Controller main routine
 */

/* INCLUDE_START ************************************************************/
#include "controller.h"
#include "cntlr_epoll.h"
#include "cntrl_queue.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "of_utilities.h"
#include "of_msgapi.h"
#include "cntlr_event.h"
#include <execinfo.h> 
#include "cntrlxml.h"
//#define _GNU_SOURCE
#include <sched.h>
#include "tmrldef.h"
#include "time.h"
#include "sll.h"
#include "memapi.h"
#include "memldef.h"
#include "dprmldef.h"
#include "cntlr_xtn.h"
#include "of_nem.h"
#include "nemldef.h"
#include <sys/time.h>
#ifdef CNTLR_CRM_SUPPORT
#if 0
#include "crmapi.h"
#include "crmldef.h"
#endif
#endif

#if 0
#include  "nsrm.h"
#include "nsrmldef.h"

#endif

struct pktbuf_callbacks *pktbuf_cbks;
/* INCLUDE_END **************************************************************/

int32_t group_id_g=0;

typedef int32_t (*CNTLRREGISTERHOOK)();
char dir_path[CNTLR_MAX_DIR_PATH_LEN+1];
/* FUNCTION_PROTOTYPE_START *************************************************/

char dir_path[CNTLR_MAX_DIR_PATH_LEN+1];

#define CNTRL_RESOURCE_CONF_FILE   "/usr/local/ond_of_driver/1.0/conf/cntrl-rconfig.xml"
void *pCntrlResourceConfigDOM_g;

#define TIMER_TYPE_CLOCK 1 

int32_t  cntlr_load_shared_libraries(char *dir_path_p);

void
of_thrd_event_handler(int32_t             fd,
    epoll_user_data_t    *info,
    uint32_t              received_events);

int32_t of_spawn_thread (char                 *thread_name,
    pthread_t                *ptid,
    OF_CNTLR_THRD_ROUTINE    thread_entry_fn,
    void                    *thread_fn_arg);

void* of_thrd_entry(void *pArg);
void thread_loop();
int32_t of_controller_init(void);
int32_t daemon_init (void);
#ifdef  CNTRL_CBENCH_SUPPORT
extern int32_t cbench_app_init();
#endif

extern void pkt_mbuf_delete_buf_pools (void);

struct peth_char_dev_info *peth_char_dev_inst_g;

/* FUNCTION_PROTOTYPE_END   *************************************************/

/*TYPEDEF_START ************************************************************/
/*TYPEDEF_END   ************************************************************/

uint32_t Lock_count =0;
/*GLOBAL_START  ************************************************************/
/** Currently not supporting multiple controller
  instnaces, only one controller instnacce is created*/ 
cntlr_instance_t cntlr_instnace; 
/** Matintance details of controler as a whole*/
cntlr_info_t  cntlr_info;

/* Master thread details of controller process*/
cntlr_thread_info_t master_thread_info;

/* State of controller process daemon*/
cntlr_dmn_run_status  cntlr_dmn_running_state;

/* Thread specific ID of Controlle process thread*/
__thread uint32_t cntlr_thread_index;

/* Thread specific  details of Controlle process thread info*/
__thread cntlr_thread_info_t *self_thread_info;

uint32_t cntlr_no_of_dps_g = 0;

/*GLOBAL_END    ************************************************************/
struct sigaction new_signal_action;
int32_t of_controller_setup_signal_handlers (void);
void of_controller_handle_signal (int16_t signal_num_i);
  int32_t
of_controller_init(void)
{

  uint8_t is_dflt_xprt_created = FALSE, is_tls_xprt_created = FALSE;
  int32_t retval = OF_SUCCESS;
  int32_t status = OF_SUCCESS;
  struct controller_transport_endpoint_info endpoint_info;

  do
  {   
    retval = timer_module_init();
    if(retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Timer module init failed\r\n");
      return retval;
    }
    of_controller_setup_signal_handlers();

    retval=of_view_init();

#if 0
#ifdef CNTLR_CRM_SUPPORT
    retval = crm_init();
    if(retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"crm_init()  - Failure");
      status = OF_FAILURE;
      break;
    }
#endif

#endif

    retval = ttp_init();
    if(retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ttp_init()  - Failure");
      status = OF_FAILURE;
      break;
    }


#if 0
    retval = nsrm_init();
    if(retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"nsrm_init()  - Failure");
      status = OF_FAILURE;
      break;
    }
    //nsrm_test();
#endif
    retval= cntlr_transprt_init();
    if(retval == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"cntlr_transprt_init()  - Failure");
      status = OF_FAILURE;
      break;
    }
#if 0
    retval = cntlr_trnsprt_layer_init();
    if(retval == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d,cntlr_trnsprt_layer_init()  - Failure\r\n",__FUNCTION__,__LINE__);
      status = OF_FAILURE;
      break;
    }
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"%s:%d,cntlr_trnsprt_layer_init()  - SUCCESS \r\n",__FUNCTION__,__LINE__);

    /* Adding Transport end point to Default port(6653) -Start */
    endpoint_info.connection_type = OF_CONN_DFLT_TRANSPORT_TYPE;
    if(cntlr_info.cntlr_portno == 0)
      endpoint_info.port            =  OF_CONN_DFLT_PORT;
    else
      endpoint_info.port            =  cntlr_info.cntlr_portno;
    endpoint_info.subjectName_of_certificate_to_use[0] = '\0';

    /* Creating default Openflow Trnasport endpoint */

    /* TBD, currently 0 is passing as controller handle, currently
     * no support to have multiple controllers
     */
    retval = dprm_add_transport_endpoint(0,&endpoint_info);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d, Creating default transport endpoint - Failure\r\n",__FUNCTION__,__LINE__);
      status = OF_FAILURE;
      break;
    }
    is_dflt_xprt_created = TRUE;
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"%s:%d, Add default Transport to Port 6653 - SUCCESS \r\n",__FUNCTION__,__LINE__);
    /* Adding Transport end point to Default port(6653) - End */

    /* Adding Transport end point to TLS - Start */
    endpoint_info.connection_type = CNTLR_TRANSPORT_TYPE_TLS;
    endpoint_info.port            = OF_CONN_TLS_PORT;

    /* Creating TLS Openflow Trnasport endpoint */

    /* TBD, currently 0 is passing as controller handle, currently
     * no support to have multiple controllers
     */
    retval = dprm_add_transport_endpoint(0,&endpoint_info);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d, Creating TLS transport endpoint - Failure\r\n",__FUNCTION__,__LINE__);
      status = OF_FAILURE;
      break;
    }
    is_tls_xprt_created = TRUE;
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d, Add TLS Transport - SUCCESS \r\n",__FUNCTION__,__LINE__);
    /* Adding Transport end point to TLS - End */
#endif
    retval = of_cntlr_msg_init();
    if(retval == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d,of_CntlrMsgInit()  - Init Failure\r\n",__FUNCTION__,__LINE__);
      status = OF_FAILURE;
      break;
    }
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d, of_cntlr_msg_init - SUCCESS \r\n",__FUNCTION__,__LINE__);


    retval=cntlr_xtn_table_init();
    if(retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d, cntlr_xtn_table_init\r\n",__FUNCTION__,__LINE__);
      status = OF_FAILURE;
      break;
    }
    cntlr_reg_domain_init();
#ifdef  CNTRL_CBENCH_SUPPORT
    retval = cbench_app_init();

    if(retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d, cbench application init failed\r\n",__FUNCTION__,__LINE__);
      status = OF_FAILURE;
      break;
    }
#endif
  }
  while(0);

  if(status == OF_FAILURE)
  {
    if(is_dflt_xprt_created == TRUE)
    {
      /*TBD, Delete default xprt*/
    }
    if(is_tls_xprt_created == TRUE)
    {
      /*TBD, Delete default xprt*/
    }
  }

  return status;
}

  void
of_thread_free_rcu(struct rcu_head *node)
{
  cntlr_thread_info_t *thread_info = (cntlr_thread_info_t*)((uint8_t*)node - sizeof(of_list_node_t));
  /*TBD, proper cleanup of the thread*/
  free(thread_info);
}

/* API returns the time elapsed by system in micro seconds as a output parameter*/
  inline int32_t
cntlr_get_time(uint64_t *timecnt)
{
  struct   timespec times;

#if TIMER_TYPE_CLOCK
  if(clock_gettime(CLOCK_REALTIME /* CLOCK_MONOTONIC*/ /*CLOCK_THREAD_CPUTIME_ID*/ ,&times) != -1) 
  {
    *timecnt =  (times.tv_sec*1000) + (times.tv_nsec/1000000.0);
    return OF_SUCCESS;
  }

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d ,Error in getting system time\r\n",__FUNCTION__,__LINE__); 
  return OF_FAILURE;
#else
  struct   timeval  timev;
  gettimeofday(&timev,NULL);
  *timecnt = (timev.tv_sec*1000) + (timev.tv_usec/1000.0);  
  return OF_SUCCESS; 
#endif
}
int main(int argc, char *argv[])
{
  int32_t                   ii, iCoreCnt = 0;
  struct sigaction          sigact;
  int32_t                    core_assgn_2_thrd;
  uint8_t                   daemonize = FALSE;
  int32_t                   retval = OF_SUCCESS;
  char                      acThNameStart[] = "OFCntlrThrd";
  cntlr_thread_info_t     *thread_info;
  char acCoreMask[128];
  char *token = NULL;
  char   acResourceConfigFile[128] = "";
  int32_t aiCoreMask[CNTLR_MAX_NUMBER_OF_CORES];
  int32_t temp;
  uchar8_t offset;
  offset = CNTLR_THREAD_INFO_LISTNODE_OFFSET;

  cntlr_info.number_of_cores = CNTLR_DFLT_NUMBER_OF_CORES;
  cntlr_info.number_of_threads = CNTLR_DFLT_NUM_TRANSPORT_THREADS;
  /*1. controller, 2. core mask, 3. number of threads, 4. port number */
  /* Format :- ./controller 1,2,4,8 16 6653 */
  self_thread_info    = &master_thread_info;
  cntlr_thread_index  = master_thread_info.thread_id = 0;
  strcpy(master_thread_info.name, "ofCntrlMainTHRD");
  for( ii = 1; ii < argc; ii++)
  {
  switch(ii)
        {
          /* Core Mask */
    case 1:
            strcpy(acCoreMask, argv[ii]);
            
            token = (char *)strtok(acCoreMask, ","); 
            do
            {
              aiCoreMask[iCoreCnt] = atoi(token);
              iCoreCnt++;
            }while( ((token = (char *)strtok(NULL,",")) != NULL) && (iCoreCnt < CNTLR_MAX_NUMBER_OF_CORES));
            cntlr_info.number_of_cores = iCoreCnt;
            for(temp = 0; temp < iCoreCnt; temp++)
              OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "%d, val: %d",temp, aiCoreMask[temp]);
      break;
          /* Number of Threads */
    case 2:
            cntlr_info.number_of_threads = atoi(argv[ii]);
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Threads: %d",cntlr_info.number_of_threads);
            break;
          /* Port Number */
    case 3:
            cntlr_info.cntlr_portno = atoi(argv[ii]);
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "port number: %d",cntlr_info.cntlr_portno);
      break;
        }
  }

  cntlr_info.num_peth_threads = CNTLR_DFLT_NUM_PETH_THREADS;

  cntlr_dmn_running_state = OF_CNTLR_DMN_STAUS_INIT;
  /* Daemonize the process */
  if(daemonize == TRUE)
  {
    daemon_init();
  }

  mempool_library_init();

  /* Register with RCU Lock library */
  CNTLR_RCU_REGISTER_THREAD();

  /* Initialize thread barriers to wait */
  /* threads and main thread wait before executing poll*/
  pthread_barrier_init(&cntlr_info.of_cntlr_barrier_process, 
      NULL,
      cntlr_info.number_of_threads+1);

  /*Initialize main thread*/
  /*Initialize ePoll Lib context*/
  if(cntlr_epoll_init(10) == OF_FAILURE)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in initializing epoll for the main thread\r\n",__FUNCTION__,__LINE__);
    exit(0);
  }  
  cntlr_info.curr_thrd_2_which_ch_assnd  = 0;
  cntlr_info.curr_core_2_which_thrd_assnd = -1;

  /*Clear CPU mask list*/
  CPU_ZERO(&cntlr_info.cpu_mask);

  OF_LIST_INIT(cntlr_info.thread_list,
      of_thread_free_rcu);
  for(ii=1; ii <= cntlr_info.number_of_threads;ii++ )
  {
    thread_info = (cntlr_thread_info_t* )of_mem_calloc(1,sizeof(cntlr_thread_info_t));
    if( thread_info == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in creating Layer Thread #%d \n\r",
          __FUNCTION__,__LINE__,ii);
      exit(0);
    }
    thread_info->thread_id = ii;
    sprintf (thread_info->name, "%s%d", acThNameStart, ii);

       core_assgn_2_thrd = ++cntlr_info.curr_core_2_which_thrd_assnd % cntlr_info.number_of_cores;
       thread_info->core_id = aiCoreMask[core_assgn_2_thrd];

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Thread Id %d Core ID to use %d\r\n",thread_info->thread_id,thread_info->core_id);

    of_spawn_thread(thread_info->name,
        &thread_info->posix_thread_id,
        of_thrd_entry,
        thread_info);

    OF_APPEND_NODE_TO_LIST (cntlr_info.thread_list, thread_info,offset);
  }

#ifdef OF_XML_SUPPORT

  /* Extract switch resource configuration information from XML configuration file */
  /* pCntrlResourceConfigDOM_g  is DOM pointer to either sw-rconfig.xml   */

  strcpy(acResourceConfigFile, CNTRL_RESOURCE_CONF_FILE);
  if(strcmp(acResourceConfigFile,"") == 0)
  {
    printf("ERROR!!! Switch Resource Configuration file NOT given !!! Exiting....\r\n");
    exit(0);
  }


  retval = CNTRLXMLLoadXML((unsigned char *)acResourceConfigFile, &pCntrlResourceConfigDOM_g);
  if(retval != OF_SUCCESS)
  {
    printf("ERROR!!! in building XML configuration!!! Exiting....\r\n");
    exit(0);
  }

  retval = execute_cntrl_resource_conf_extract(pCntrlResourceConfigDOM_g);
  if(retval != OF_SUCCESS)
  {
    printf("ERROR!!! in extracting sw resource configuration!!! Exiting....\r\n");
    exit(0);
  }
#endif

  /* Register init hooks of shared library modules */
  memset(dir_path, 0, CNTLR_MAX_DIR_PATH_LEN);
  strcpy(dir_path, CNTLR_DIR);
  strcat(dir_path, CNTLR_SHARED_INFRA_DIR);
  cntlr_load_shared_libraries(dir_path);
  
  retval = dprm_init();
  if(retval == OF_FAILURE)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," of_contrroler_init() - Failure");
    exit(0);
  }
  
  memset(dir_path, 0, CNTLR_MAX_DIR_PATH_LEN);
  strcpy(dir_path, CNTLR_DIR);
  strcat(dir_path, CNTLR_SHARED_UTILS_DIR);
  cntlr_load_shared_libraries(dir_path);

  retval = of_controller_init();
  if(retval == OF_FAILURE)
  {
     OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"cntlr_transprt_init()  - Failure");
     exit(0);
  }

  /* Register init hooks of shared library modules */
  memset(dir_path, 0, CNTLR_MAX_DIR_PATH_LEN);
  strcpy(dir_path, CNTLR_DIR);
  strcat(dir_path, CNTLR_SHARED_APP_DIR);
  cntlr_load_shared_libraries(dir_path);

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"barrier wait...");
  retval = pthread_barrier_wait(&cntlr_info.of_cntlr_barrier_process);
  if( retval != 0 && retval != PTHREAD_BARRIER_SERIAL_THREAD)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s: Could not wait on barrier\n",__FUNCTION__);
    exit(0);
  }

  cntlr_dmn_running_state = OF_CNTLR_DMN_STAUS_RUNNING;

  /* Go into Execution loop  */
#ifdef CNTRL_OFCLI_SUPPORT
      cntrl_ucm_init();
#endif
  thread_loop();
  cntlr_epoll_shutdown();
  exit(0);


  return OF_SUCCESS;
}

int32_t of_spawn_thread (char                 *thread_name,
    pthread_t                *ptid,
    OF_CNTLR_THRD_ROUTINE    thread_entry_fn,
    void                    *thread_fn_arg)
{
  int32_t         retval;
  pthread_attr_t  attr;

  pthread_attr_init (&attr);
  pthread_attr_setschedpolicy (&attr, SCHED_RR);

  /* Create and spawn the thread */
  retval =  pthread_create (ptid, &attr , thread_entry_fn,
      (void *) thread_fn_arg);
  return retval;
}

  void*
of_thrd_entry(void *pArg)
{
  cntlr_thread_info_t       *thread_info;
  epoll_user_data_t          epoll_usr_data;
  cpu_set_t                  cpu_mask;
  uint32_t                   interested_events;
  int32_t                    dyn_sock_handle;
  uint16_t                   dyn_loopback_port;
  int32_t                    retval = OF_SUCCESS;
  int32_t                    status = OF_SUCCESS;

  /* Store thread index  and thread info in thread specific variable */
  thread_info                  =   (cntlr_thread_info_t *)pArg;
  cntlr_thread_index           =   thread_info->thread_id;
  self_thread_info             =   thread_info;

  CNTLR_RCU_REGISTER_THREAD();

  do
  {
#if 1
    if(thread_info->thread_id != 0)
    {
       OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Thread Id %d core Id %d",thread_info->thread_id,thread_info->core_id);

      CPU_ZERO(&cpu_mask);
       CPU_SET(thread_info->core_id, &cpu_mask);
      retval = pthread_setaffinity_np(pthread_self(),
          sizeof(cpu_mask),
          &cpu_mask);
      if ( retval !=  0)
      {
        perror("pthread_setaffinity_np");
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," pthread_setaffinity_np() failed for Thread # %u , Core = %u !!!! ",cntlr_thread_index,thread_info->core_id);
        status = OF_FAILURE;
        break;
      }
    }

    CPU_SET(thread_info->core_id, &cntlr_info.cpu_mask);
#endif

    /*TBD of proper def at proper place*/
    retval = cntlr_epoll_init(NO_FD_SUPPORTED);
    if( retval != OF_SUCCESS )
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d  error 0x%x in initializing the ePoll library\r\n",__FUNCTION__,__LINE__, retval);
      status = OF_FAILURE;
      break;
    }

#if 1
    /* Create loopback socket, bind to a port */
    thread_info->loopback_port    = 0;
    thread_info->loopback_sock_fd = 0;
    retval = of_get_ephermal_port(OF_IPPROTO_UDP,
        OF_LOOPBACK_SOCKET_IPADDR,
        &dyn_sock_handle,
        &dyn_loopback_port);
    if( retval != OF_SUCCESS )
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d error 0x%x in getting an ephermal loopback port\r\n",
          __FUNCTION__,__LINE__,
          retval);
      pthread_exit(NULL);
    }
    thread_info->loopback_port    = dyn_loopback_port;
    thread_info->loopback_sock_fd = dyn_sock_handle;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d  loopback_port = %d  \n",__FUNCTION__,__LINE__,dyn_loopback_port);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d loopback_sock_fd = 0x%x \n",__FUNCTION__,__LINE__,dyn_sock_handle);

    /* Add Loopback FD to Poll table using API ‘epoll_add_fd’ */
    memset(&epoll_usr_data,0,sizeof(epoll_user_data_t));
    epoll_usr_data.arg1 = (void *)thread_info;
    epoll_usr_data.arg2 = NULL;
    interested_events = ePOLL_POLLIN;
    retval = cntlr_epoll_add_fd(thread_info->loopback_sock_fd,
        interested_events,
        &epoll_usr_data,
        of_thrd_event_handler);
    if(retval == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Error in adding Thread Socket to EPOLL\r\n", __FUNCTION__,__LINE__);
      pthread_exit(NULL);
    }
#endif
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"barrier wait...");
    retval = pthread_barrier_wait(&cntlr_info.of_cntlr_barrier_process);
    if( retval != 0 && retval != PTHREAD_BARRIER_SERIAL_THREAD)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s: Could not wait on barrier, Thread ID %d\n", __FUNCTION__,cntlr_thread_index);
      status = OF_FAILURE;
      break;
    }

  } while(0);

  if(status == OF_FAILURE)
  {
    cntlr_epoll_shutdown();
#if 0
    /*TBD*/
    OF_REMOVE_NODE_FROM_LIST(cntlr_info.thread_list, );
#else
    free(thread_info);
#endif

    pthread_exit(NULL);
  }
  thread_loop();
  cntlr_epoll_shutdown();
#if 0
  /*TBD*/
  OF_REMOVE_NODE_FROM_LIST(cntlr_info.thread_list, );
#else
  free(thread_info);
#endif
  pthread_exit(NULL);
}

  void
thread_loop()
{
  int32_t  wait_time = -1;
  uint64_t time1=0, time2=0;

  uint32_t next_expiry_timer_remval = 0;
  uint64_t elapsed_time;

  while(TRUE)
  { 
    next_expiry_timer_remval = (__timer_get_least_granularity() / 10000);  /* divisor = 10000 results in 10msec */
    if(next_expiry_timer_remval == 0 )
    {
      wait_time = 20;
    }
    else
    {
      wait_time = next_expiry_timer_remval;
    }
    cntlr_get_time(&time1);

    cntlr_epoll_wait(wait_time);

    cntlr_get_time(&time2);

    elapsed_time = (time2 - time1);

    /* Timer Processing */
    if((next_expiry_timer_remval) && (elapsed_time))
      timer_process_timers();
  }
}

  void
of_thrd_event_handler(int32_t             fd,
    epoll_user_data_t    *info,
    uint32_t              received_events)
{
  cntlr_thread_info_t *thread_info = (cntlr_thread_info_t*)info->arg1;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Not implemented Event Handler %d \r\n",
      __FUNCTION__,__LINE__,thread_info->thread_id);
}


int32_t of_show_stack_trace()
{
  void *callstack[128];
  int32_t i,frames = backtrace(callstack, 128);

  char** strs = backtrace_symbols(callstack, frames);
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"** START of Stack Trace **\r\n");
  for (i=0; i < frames; ++i)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"frame #%d: %s \r\n",i, strs[i]);
  }

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"** END of Stack Trace **\r\n");

  free(strs);
  return 0;
}

static void of_ctlr_signal_handler(int32_t sig_num)
{
  struct sigaction old_action;


  if( sigaction (sig_num, NULL, &old_action) == -1)
    perror("Failed to get old Handle");

  if(sig_num == SIGHUP)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s::%d Recieved the Hangup Signal\n\r",__func__,__LINE__);
  }

  if(sig_num == SIGSEGV)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s::%d Recieved the SIGSEGV Signal\n\r",__func__,__LINE__);
  }

  if(sig_num == SIGINT)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s::%d Recieved the Interuppt Signal\n\r",__func__,__LINE__);
    /* To generate reports like memory checks some tools like valgrind expect normal termination
    *  of process, to facilitate this we are calling exit(0) */
    exit(0);
  }

  if(sig_num == SIGTERM)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s::%d Recieved the TERM Kill Signal\n\r",__func__,__LINE__);
  }

  if(sig_num == SIGTSTP)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s::%d Recieved the keyboard control Z Signal\n\r",__func__,__LINE__);
  }

  if(sig_num == SIGKILL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s::%d Recieved the kill Signal\n\r",__func__,__LINE__);
  }

  if(sig_num == SIGILL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s::%d Recieved the SIGILL Signal\n\r",__func__,__LINE__);
  }

  if(sig_num == SIGABRT)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s::%d Recieved the SIGABRT Signal\n\r",__func__,__LINE__);
  }
}

int32_t of_socket_create(uint8_t porotocol)
{
  int32_t sock_fd;

  switch(porotocol)
  {
    case OF_IPPROTO_TCP:
      sock_fd = socket(AF_INET, SOCK_STREAM, porotocol);
      break;
    case OF_IPPROTO_UDP:
      sock_fd = socket(AF_INET, SOCK_DGRAM, porotocol);
      break;
    case OF_IPPROTO_ICMP:
      sock_fd = socket(AF_INET,SOCK_RAW,porotocol);
      break;
    case OF_IPPROTO_IGMP:
      sock_fd = socket(AF_INET,SOCK_RAW,porotocol);
      break;
    default:
      return OF_FAILURE;
  }

  if( sock_fd < 0 )
    return OF_FAILURE;
  else  
    return sock_fd;
}

int32_t  of_get_ephermal_port( uint8_t        protocol,
    uint32_t       ip_addr,
    int32_t       *sock_fd,
    uint16_t      *port_number
    )
{
  struct sockaddr_in local_addr;
  struct sockaddr_in my_addr;
  uint32_t           my_addr_len;
  int32_t            sock_handle = OF_FAILURE;
  int32_t            retval      = OF_SUCCESS;
  int32_t            status      = OF_SUCCESS;

  do
  {
    retval = of_socket_create(protocol);
    if( retval == OF_FAILURE )
    {
      status = OF_FAILURE;
      break;
    }
    sock_handle = retval;

    memset((char*)&local_addr,0,sizeof(local_addr));
    local_addr.sin_family      = AF_INET;
    local_addr.sin_addr.s_addr = htonl(ip_addr);
    local_addr.sin_port        = htons(INADDR_ANY);
    retval = bind(sock_handle, (struct sockaddr *)&local_addr, sizeof(struct sockaddr_in));
    if(retval == OF_FAILURE)
    {
      status = OF_FAILURE;
      break;
    }

    my_addr_len = sizeof(my_addr);
    retval = getsockname( sock_handle, 
        (struct sockaddr*)&my_addr,
        (socklen_t *)&my_addr_len );
    if(retval == OF_FAILURE)
    {
      status = OF_FAILURE;
      break;
    }

  } while(0);

  if( status == OF_SUCCESS )
  {
    *sock_fd     = sock_handle;
    *port_number = ntohs(my_addr.sin_port);
  }
  else
  {
    if( sock_handle != OF_FAILURE )
    {
      close(sock_handle);
      sock_handle = OF_FAILURE;
    }
  }

  return status;
}

  cntlr_thread_info_t*
cntlr_get_thread_info( uint32_t thread_id)
{
  cntlr_thread_info_t *thread_info =NULL;
  uchar8_t offset;
  offset = CNTLR_THREAD_INFO_LISTNODE_OFFSET;

  OF_LIST_SCAN(cntlr_info.thread_list, thread_info, cntlr_thread_info_t*,offset)
  {
    if(thread_info->thread_id == thread_id)
      return thread_info;
  }
  return NULL;
}

/* Load each shared library file (*.so) found in the given path and invoke Registration hook */
int32_t  cntlr_load_shared_libraries(char *dir_path_p)
{
  char    *error;
  int32_t ii,no_of_files;
  struct  dirent **a_file_names_p_p;
  char    path[CNTLR_MAX_DIR_PATH_LEN+1]={'\0'};
  void    *app_library_hndl_p;

  CNTLRREGISTERHOOK ThreadInitHook; 

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Shared Lib DIR Path = %s",dir_path_p);
  no_of_files = scandir(dir_path_p,&a_file_names_p_p, 0, alphasort);
  if( no_of_files < 0  )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN, "scandir failed for DIR path: %s", dir_path_p);
    return OF_FAILURE;
  }

  for( ii = 0; ii < no_of_files; ii++)
  {
    /*Get the next file */
    if(!(strcmp( a_file_names_p_p[ii]->d_name,"."))  || !(strcmp(a_file_names_p_p[ii]->d_name,"..")))
    {
      free(a_file_names_p_p[ii]);
      continue;
    }
    if(a_file_names_p_p[ii]->d_name[0] != 's')
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "first character is not s but %c",a_file_names_p_p[ii]->d_name[0]);   
      continue;
    } 
    if((a_file_names_p_p[ii]->d_name[1] < '0') || (a_file_names_p_p[ii]->d_name[1] > '9'))
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "2nd character is not a digit but %c",a_file_names_p_p[ii]->d_name[1]);
      continue;
    }
    if((a_file_names_p_p[ii]->d_name[2] < '0') || (a_file_names_p_p[ii]->d_name[2] > '9'))
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "3rd character is not a digit but %c",a_file_names_p_p[ii]->d_name[2]);
      continue;
    }

    snprintf (path,CNTLR_MAX_DIR_PATH_LEN,"%s/%s",dir_path_p,a_file_names_p_p[ii]->d_name);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"path:%s",path);

    /* Open the file */
    app_library_hndl_p = dlopen(path,RTLD_NOW|RTLD_GLOBAL);

    if(!app_library_hndl_p)
    {
      if ((error = dlerror()) != NULL)
      {
        fprintf (stderr, "%s:%s\n",__FUNCTION__,error);
      }
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "dlopen failed for path:%s\n",path);
      continue;
    }

    /* Get the Registration Handler */
    ThreadInitHook = ( CNTLRREGISTERHOOK)dlsym(app_library_hndl_p,"cntlr_shared_lib_init");
    if ((error = dlerror()) != NULL)
    {
      fprintf (stderr, "%s:dlsym() returned error: %s\n",__FUNCTION__,error);
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "dlsym failed for path:%s\n",path);
      continue;
    }

    /*  Invoke the Resgistration hook */
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Invoking next hook \n");
    if(ThreadInitHook)
      ThreadInitHook();
  }

  free(a_file_names_p_p);
  return OF_SUCCESS;
}



int32_t of_controller_setup_signal_handlers (void)
{  

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered");

  new_signal_action.sa_handler = of_controller_handle_signal;
  sigemptyset (&new_signal_action.sa_mask);
  new_signal_action.sa_flags =  0;
  new_signal_action.sa_flags =  new_signal_action.sa_flags | SA_NODEFER | SA_RESETHAND;

  signal(SIGPIPE, SIG_IGN);
  if( sigaction (SIGQUIT, &new_signal_action, NULL) == -1)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN, "Failed to set new Handle");
    perror("Failed to set new Handle SIGQUIT");
  }
  if( sigaction (SIGINT, &new_signal_action, NULL) == -1)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN, "Failed to set new Handle");
    perror("Failed to set new Handle SIGINT ");
  }
  if( sigaction (SIGTERM, &new_signal_action, NULL) == -1)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN, "Failed to set new Handle");
    perror("Failed to set new Handle SIGTERM ");
  }
  if( sigaction (SIGHUP, &new_signal_action, NULL) == -1)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN, "Failed to set new Handle");
    perror("Failed to set new Handle SIGHUP ");
  }
  if( sigaction (SIGSEGV, &new_signal_action, NULL) == -1)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN, "Failed to set new Handle");
    perror("Failed to set new Handle SIGSEGV ");
  }
  if( sigaction (SIGILL, &new_signal_action, NULL) == -1)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN, "Failed to set new Handle");
    perror("Failed to set new Handle SIGILL");
  }
  if( sigaction (SIGABRT, &new_signal_action, NULL) == -1)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN, "Failed to set new Handle");
    perror("Failed to set new Handle SIGABRT ");
  }


  return OF_SUCCESS;
}     

void of_controller_handle_signal (int16_t signal_num_i)
{
  struct sigaction old_action;


  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered");

  cntlr_transprt_deinit();

  dprm_namespace_deinit();

 /* add all clean up/De-Init/De-Registration functions here */

  if( sigaction (signal_num_i, NULL, &old_action) == -1)
    perror("Failed to get old Handle");

   switch(signal_num_i)
  {

    case SIGHUP:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Recieved the Hangup Signal");
      }
      break;
    case SIGSEGV:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Recieved the SIGSEGV Signal");
      }
      break;
    case SIGINT:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Recieved the Interuppt Signal");
        /* To generate reports like memory checks some tools like valgrind expect normal termination
         *  of process, to facilitate this we are calling exit(0) */
      }
      break;
    case SIGTERM:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Recieved the TERM Kill Signal");
      }
      break;
    case SIGTSTP:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Recieved the keyboard control Z Signal");
      }
      break;
    case SIGKILL:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Recieved the kill Signal");
      }
      break;
    case SIGILL:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Recieved the SIGILL Signal");
      }
      break;
    case SIGABRT:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Recieved the SIGABRT Signal");
      }
      break;
  }

      exit(0);
}




int32_t cntlr_register_pktbuf_callbacks(
		struct pktbuf_callbacks *pktbuf_callbacks_p)
{     
	int32_t                 status = OF_SUCCESS;
	int32_t                 retval = OF_SUCCESS;

	OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_DEBUG,"entered");

	do
	{

		if ( pktbuf_callbacks_p == NULL )
		{
			OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"Invalid callback pointer");
			status = OF_FAILURE;
			break;
		}

		//cntlr_instance.pktbuf_cbks = pktbuf_callbacks_p;
		pktbuf_cbks = pktbuf_callbacks_p;
	}
	while(0);

	return status;
}


