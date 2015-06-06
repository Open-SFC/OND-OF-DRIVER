/**
 *  Copyright (c) 2012, 2013  Freescale.
 *   
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 **/

/* File: controller.h
 * 
 * Description: Common definitions  of Controller 
 * 
 * Authors:     Rajesh Madabushi <rajesh.madabushi@freescale.com>
 */
#ifndef __CONTROLLER_H
#define __CONTROLLER_H

#ifndef __USE_GNU
#define __USE_GNU 1
#endif

/** INCLUDE_START  */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>
#include <dirent.h>
#include <dlfcn.h>

#include <urcu/arch.h>
#include <urcu.h>
#include <urcu-call-rcu.h>
#include "linux/futex.h"
#include "usersem.h"


#include "cntlr_system.h"
#include "cntlr_types.h"
#include "memapi.h"
#include "cntlr_error.h"
#include "cntlr_lock.h"
#include "cntlr_list.h"
#include "mchash.h"

#include "openflow.h"
#include "of_utilities.h"
#include "of_msgapi.h"
#include "dprm.h"
#include "timerapi.h"
#include "oflog.h"
//#include "of_msgpool.h"
//#include "of_msgapi.h"
//#include "of_multipart.h"

/** INCLUDE_END ******/

/** TYPEDEF_START  **/
typedef void *  (*OF_CNTLR_THRD_ROUTINE)(void *);

typedef enum
{
OFPMP_IPOPTS_DESC= OFPMP_PORT_DESC +1,
}cntrl_async_requests;
//#define CNTLR_DIR "/home/suram/OpenFlow/controller_3003"
//#define CNTLR_SHARED_APP_DIR  "/build/lib/apps"
//#define CNTLR_MAX_DIR_PATH_LEN  84

typedef enum 
{
  OF_CNTLR_DMN_STAUS_INIT =1, 
  OF_CNTLR_DMN_STAUS_RUNNING
} cntlr_dmn_run_status;


#define OF_THRD_NAME_LEN  (32)
/** This structure defines details of each Conntroller Thread */
typedef struct cntlr_thrd_info_s
{
  of_list_node_t next; /** Pointer to next thread Id*/
  struct rcu_head  rcu_head;
  pthread_t       posix_thread_id; /** Thread Id returned by OS*/
  int32_t        thread_id; /** Thread Id assigned by us */
  char           name[OF_THRD_NAME_LEN+1]; /** Name of the thread */
  uint32_t       core_id; /*ID of the core afined, it is  appicable only for worker thread afined to the core*/
  void          *p_epoll_lib_cntxt; /** epoll context to receive events*/
  int32_t        loopback_sock_fd; /** Loopback socket fd to receive events */ 
  uint16_t       loopback_port; /** Port number on which thread receieves events*/
} cntlr_thread_info_t;
#define CNTLR_THREAD_INFO_LISTNODE_OFFSET offsetof(cntlr_thread_info_t, next)

/** Number of cores used by system Controller */
#define CNTLR_NO_OF_CORES_IN_SYSTEM            (cntlr_info.number_of_cores)

#define LINUX_PSEUDO_ETH_STARTING_CORE_ID	(4)

#define LINUX_PSEUDO_ETH_NUM_CORES		(4)


#define CNTLR_DFLT_NUM_TRANSPORT_THREADS  4 

#define CNTLR_DFLT_NUM_PETH_THREADS  4

#define CNTLR_NUM_TRANSPORT_THREADS	(cntlr_info.number_of_threads)

/* One for Routing listener, one for Ipsec listener */
#define CNTLR_NUM_APPLICATION_THREADS	(2)

#define CNTLR_NUM_PETH_THREADS		(cntlr_info.num_peth_threads)

/** Maximum number of threads created by Controller, this number includes the
 *  transport threads, peth threads and the application threads.
 *  Main thread is not computed in this macro.
 */
#define CNTLR_NO_OF_THREADS_IN_SYSTEM  (cntlr_info.number_of_threads + cntlr_info.num_peth_threads + CNTLR_NUM_APPLICATION_THREADS)

#define CNTLR_MAX_NSIDS		(512)

/** Data_structure holding controller global infor*/
typedef  struct  ctlr_info_s
{                        
  uint32_t          process_id; /** Process ID of controller daemon*/
  uint32_t          number_of_threads; /** Number of transport threads created by Controller daemon*/
  pthread_barrier_t of_cntlr_barrier_process; /*Thread barrier used to synchronization between transport threads*/
  uint32_t          curr_thrd_2_which_ch_assnd; /** This is used in load balancing of transport threads,
                                                    indicates next available transport thread to assign new
                                                    channeld created*/
  int32_t           curr_core_2_which_thrd_assnd; /* This is used in load balancing of the cores between transport threads.
                                                     This indicates cur core number to which transport thread assigned*/
  uint32_t          number_of_cores; /* Number of cores supported by the system*/
												
  cpu_set_t         cpu_mask; /*Set of CPU Cores used for the process*/ 
  of_list_t         thread_list; /** List of transport threads, it is of_cntlr_thrd_info_t*/

  pthread_barrier_t peth_app_barrier_process; /*Thread barrier used to synchronization between peth threads*/
  uint32_t          num_peth_threads; /** Number of peth app threads created by Controller daemon*/
  of_list_t         peth_thread_list; /** List of peth app threads, it is of_cntlr_thrd_info_t*/

  uint32_t          cntlr_portno; /** Process ID of controller daemon*/
}cntlr_info_t;

/** Data structure holding details of each controller instance,
 * right now only one controller instance will be created, this is design provision
 * for future
 */

typedef	struct pkt_mbuf * (* pktbuf_alloc_p)(uint32_t uiDataSize, uint32_t uiFlags);
typedef int32_t (* pktbuf_free_p) (struct pkt_mbuf *pBuf);
struct pktbuf_callbacks
{
	pktbuf_alloc_p  pktbuf_alloc;
	pktbuf_free_p  pktbuf_free;
};
typedef struct cntlr_instance_s
{ 
   of_list_t tranprt_list; /** List of transports supported by controller*/
   of_list_t dp_list;  /** List of data_paths supported by controller*/
   of_list_t mempool_list;  /** List of data_paths supported by controller*/
//  struct pktbuf_callbacks *pktbuf_cbks;
} cntlr_instance_t;


/** Data structure that carry details of command  sending to thread*/
typedef struct cntlr_thread_comm_info_s
{
  uint32_t command; /** Command ID, depending on the type of command union carry corresponding
                     * data_structure*/
  union
  {
    uint32_t sender_thread_id; /** Thread ID of the thread which sending the command*/
  }msg;
}cntlr_thread_comm_info_t;

typedef struct of_data_path_id_s
{
    union
    {
       uint32_t ul[2];
       uint64_t val;
       struct
       {
          uint32_t macLo;
          uint16_t macHi;
          uint16_t x;
       };
    };
} of_data_path_id_t;


/** Error Structure that carries error type, code and its corresponding printable string */
typedef struct of_error_strings_s{
	uint16_t type;
	uint16_t code;
	char error_str[300];

}of_error_strings_t;

/* TYPEDEF_END */ 
#if 0

//typedef unsigned long cpu_set_t;
# define CPU_ZERO(cpuset) do { *(cpuset) = 0; } while(0)
# define CPU_SET(cpu, cpuset) do { *(cpuset) |= (1UL << (cpu)); } while(0)
#endif

#define NO_FD_SUPPORTED 50
#define TOTAL_ERROR_MSGS_COUNT 110

#define OF_STORE_DATAPATH_ID(Id,Value)\
{\
   Id.ul[0] = ((unsigned long)Value);\
   Id.ul[1] = ((unsigned long)(Value >>32));\
}

#define OF_NTOHLL_DP_ID(Id,Value)\
{\
   Id.ul[0] = ntohl((uint32_t)(Value >>32));\
   Id.ul[1] = ntohl((uint32_t)Value);\
}


#if __BYTE_ORDER == __BIG_ENDIAN
#define ntohll(val) val
#else
#define ntohll(ull) (((((uint64_t)(ntohl((uint32_t)(ull))) )&(0x00000000ffffffff))<<32)\
                  + (((uint64_t)(ntohl((uint32_t)(ull>>32))) &(0x00000000ffffffff))))
#endif

#if __BYTE_ORDER == __BIG_ENDIAN
#define htonll(val) val
#else
#define htonll(ull) (((((uint64_t)(htonl((uint32_t)(ull))) )&(0x00000000ffffffff))<<32)\
                  + (((uint64_t)(htonl((uint32_t)(ull>>32))) &(0x00000000ffffffff))))
#endif

#ifndef __KERNEL__
#ifdef __GNUC__
#ifndef likely
#define likely(a)       __builtin_expect(!!(a), 1)
#endif
#ifndef unlikely
#define unlikely(a)     __builtin_expect(!!(a), 0)
#endif
#else
#ifndef likely
#define likely(a)       (a)
#endif
#ifndef unlikely
#define unlikely(a)     (a)  
#endif
#endif
#endif

#define CNTLR_LIKELY(a)   likely(a)
#define CNTLR_UNLIKELY(a) unlikely(a)

#define CNTLR_PKTPROC_FUNC __attribute__ ((section("cntlrcode")))
#define CNTLR_PKTPROC_DATA __attribute__ ((section("cntlrdata")))

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define OF_SUCCESS   0
#define OF_FAILURE  -1

/*Flag used to create periodic timer*/
#define CNTLR_PEIRODIC_TIMER      TRUE 
#define CNTLR_NOT_PEIRODIC_TIMER  FALSE 

/*flag used to create one time timer*/
#define CNTLR_TIMER_ONCE           FALSE

/**Memory related APIs*/
#define of_mem_calloc calloc
#define of_mem_free   free

//#define of_debug printf

/*TBD, need to have provision to configure no of cores in the system avilable controller*/

#define OF_MAX_NO_OF_CTRLR_INSTANCES               1
#define OF_MAX_NO_OF_DPs_EACH_CTRLR_INST          25
#define OF_MAX_NO_OF_CHANNELS_WITHIN_DP           10

#define OF_EPOLL_CTXT_MAX_FDS_SUPPORTED\
     ((OF_MAX_NO_OF_CTRLR_INSTANCES *OF_MAX_NO_OF_DPs_EACH_CTRLR_INST) + 20)

#if 0
/*TBD need to set this env varialble, currently using ony Little endian, find  def below */
#if __BYTE_ORDER == __BIG_ENDIAN
#define OF_NTOHLL(Id,Value) OF_STORE_DATAPATH_ID(Id,Value)
#else
#if __BYTE_ORDER == _LITTLE_ENDIAN
#define OF_NTOHLL(Id,Value)\
{\
ab   Id.ul[0] = ntohl((uint32_t)(Value >>32));\
   Id.ul[1] = ntohl((uint32_t)Value);\
}
#endif
#endif
#endif

#if 0
#if __BYTE_ORDER == __BIG_ENDIAN
#define OF_HTONLL(Id,Value) OF_STORE_DATAPATH_ID(Id,Value)
#else
#if __BYTE_ORDER == _LITTLE_ENDIAN
#define OF_HTONLL(Id) (((uint64_t)(htonl((uint32_t)Id)) << 32 | htonl((uint32_t)(Id >> 32))))
#endif
#endif
#endif

#define OF_DOBBS_MIX(a,b,c) \
{ \
    a -= b; a -= c; a ^= (c>>13); \
    b -= c; b -= a; b ^= (a<<8);  \
    c -= a; c -= b; c ^= (b>>13); \
    a -= b; a -= c; a ^= (c>>12); \
    b -= c; b -= a; b ^= (a<<16); \
    c -= a; c -= b; c ^= (b>>5);  \
    a -= b; a -= c; a ^= (c>>3);  \
    b -= c; b -= a; b ^= (a<<10); \
    c -= a; c -= b; c ^= (b>>15); \
}


#define ROUND_UP(len) ((len + 7)/8*8 - len)

#define OF_DOBBS_WORDS3(word_a, word_b, word_c, initval, \
                         hashtblsz, hash) \
{ \
    register uint32_t temp_a, temp_b, temp_c; \
    temp_a = temp_b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */\
    temp_c = initval;         /* random value*/ \
  \
    temp_a += word_a; \
    temp_b += word_b; \
    temp_c += word_c; \
  \
    OF_DOBBS_MIX(temp_a, temp_b, temp_c); \
  \
    hash = temp_c % hashtblsz; \
}

#define OF_DOBBS_WORDS2(word_a, word_b, initval, hashtblsz, hash) \
{ \
    register uint32_t temp_a, temp_b, temp_c; \
    temp_a = temp_b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */\
    temp_c = initval;         /* random value*/ \
  \
    temp_a += word_a; \
    temp_b += word_b; \
  \
    OF_DOBBS_MIX(temp_a, temp_b, temp_c); \
  \
    hash = temp_c % hashtblsz; \
}


#ifndef INADDR_ANY
#define INADDR_ANY ((unsigned long int ) 0x00000000)
#endif /* INADDR_ANY */

#define OF_LOOPBACK_SOCKET_IPADDR ((unsigned long int ) 0x7f000001)


/** Protocol definitions*/
enum {
   OF_IPPROTO_ICMP  =  1,
   OF_IPPROTO_IGMP  =  2,
   OF_IPPROTO_IPIP =  4,
   OF_IPPROTO_TCP  =  6,
   OF_IPPROTO_EGP =  8,
   OF_IPPROTO_UDP  =  17,
   OF_IPPROTO_RSVP =  46,
   OF_IPPROTO_GRE =  47,
   OF_IPPROTO_ESP =  50,
   OF_IPPROTO_AH =  51,
   OF_IPPROTO_SCTP =  132,
   OF_IPPROTO_MOBILE_IP = 135,
   OF_IPPROTO_UDPLITE =  136,
   OF_IPPROTO_IPCOMP = 108,
   OF_IPPROTO_IPV6 = 41,

   /* special codes */
   OF_IPPROTO_OTHER  =  254,
   OF_IPPROTO_ALL  =  255,
};

/** Controller Role defintions*/
#define OF_CNTLR_ROLE_EQUAL       (0)
#define OF_CNTLR_ROLE_MASTER      (1)
#define OF_CNTLR_ROLE_SLAVE       (2)
#define OF_CNTLR_ROLE_DEFAULT     OF_CNTLR_ROLE_EQUAL

#define cntlr_assert(cond)  do { \
                                if(!(cond)) \
                                { \
                                  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_CRITICAL,"ASSERT: (%s) failed @ %s:%d\n",#cond,__FILE__,__LINE__); \
                                  of_show_stack_trace();\
                                } \
                             } while(0);

/** Macros used to build message buffers. For a given Open Flow message or
* packet in the message, this macro  used build the messages in buffer 
* given as ouptut paramter.  One can be used it to print the message buffers*/
#define CNTLR_MAX_NO_OF_BYTES_PRINT 128
#define CNTLR_SIZE_OF_DATA_TO_PRINT_ON_EACH_LINE 16
#define CNTLR_MAX_NO_OF_LINES  (CNTLR_MAX_NO_OF_BYTES_PRINT/CNTLR_SIZE_OF_DATA_TO_PRINT_ON_EACH_LINE) + 1;      
#define CNTLR_MAX_MSG_BUF_SIZE  ((3*CNTLR_MAX_NO_OF_BYTES_PRINT) + (2*CNTLR_MAX_NO_OF_LINES) + 10) 
#define CNTLR_BUILD_MSG_BUF_2_PRINT(data,data_len,buf,max_buf_size)\
{\
   uint8_t *data_ptr;\
   uint8_t *buf_ptr;\
   uint16_t buf_byte;\
   uint16_t data_byte;\
\
   for(buf_byte = 0,data_byte = 0,buf_ptr = buf,data_ptr=data;\
       ((data_byte < data_len) && (buf_byte < max_buf_size)); \
       data_byte++,data_ptr++,buf_byte++)\
   {\
      sprintf((char *)buf_ptr,"%02x ",*data_ptr);\
      buf_ptr += 3;\
      buf_byte += 3;\
      if((data_byte != 0) && ((data_byte % CNTLR_SIZE_OF_DATA_TO_PRINT_ON_EACH_LINE) == 0)) \
      {\
         sprintf((char *)buf_ptr,"\r\n");\
         buf_ptr += 2;\
         buf_byte += 2;\
      }\
   }\
   buf[buf_byte] = '\0';\
}
 
#define CNTLR_DUMP_MSG_DATA(dpid,msg)\
{\
   struct ofp_header *header = (struct ofp_header *)(msg->desc.start_of_data);\
   uint8_t *data;\
   uint8_t  buf[512];\
   CNTLR_BUILD_MSG_BUF_2_PRINT(msg->desc.start_of_data,ntohs(header->length),buf,511);\
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"DP ID %llx Type %d Len %d xid %d\r\n%s\r\n",\
                                   dpid,header->type,ntohs(header->length),ntohl(header->xid),buf);\
}

#define CNTLR_DUMP_MSG_RCVD_ON_XPRT CNTLR_DUMP_MSG_DATA

#define CNTLR_PKT_IN_DATA(dpid,pkt_in_msg)\
{\
   uint8_t  buf[256];\
   CNTLR_BUILD_MSG_BUF_2_PRINT(pkt_in_msg.packet,pkt_in_msg.packet_length,buf,255);\
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"DP ID %llx\r\n frame %s\r\n",dpid,buf);\
}\

#define PSP_PKTPROC_FUNC	CNTLR_PKTPROC_FUNC
#define PSP_PKTPROC_DATA	CNTLR_PKTPROC_DATA
#define PSP_LIKELY(a) likely(a)
#define PSP_UNLIKELY(a) unlikely(a)

#define OF_GET_ERROR_STRING(error_type,error_code,error_string)\
{\
 uint16_t i;\
 for(i=0;i < TOTAL_ERROR_MSGS_COUNT;i++){\
     if((error_type == of_error_printable_string_data[i].type) && (error_code == of_error_printable_string_data[i].code)){\
       strcpy(error_string,of_error_printable_string_data[i].error_str);\
       break;\
     }\
 }\
     if(i >= TOTAL_ERROR_MSGS_COUNT)\
	strcpy(error_string,"UNKNOWN ERROR MESSAGE");\
}


/**** DEFINE_END  */

/*GLOBAL_START **************************************************************/
extern cntlr_instance_t cntlr_instnace; 
extern cntlr_info_t  cntlr_info;
extern cntlr_thread_info_t master_thread_info;
extern cntlr_dmn_run_status  cntlr_dmn_running_state;
extern __thread uint32_t cntlr_thread_index;
extern __thread cntlr_thread_info_t *self_thread_info;

extern uint32_t cntlr_no_of_dps_g;

extern cntlr_instance_t cntlr_instnace;

extern uint64_t controller_handle;

/*Error String array **/
extern of_error_strings_t of_error_printable_string_data[];


/*GLOBAL_END   **************************************************************/

/*FUNCTION_PROTOTYPE_START *************************************************/
cntlr_thread_info_t* cntlr_get_thread_info( uint32_t thread_id);
int32_t of_socket_create(uint8_t porotocol);
int32_t  of_get_ephermal_port( uint8_t        protocol,
                               uint32_t       ip_addr,
                               int32_t       *sock_fd,
                               uint16_t      *port_number
                              );
int32_t cntlr_trnsprt_layer_init(void);
int32_t of_cntlr_msg_init();
int32_t of_pkt_buf_init ();
int32_t config_resources();
int32_t of_show_stack_trace();
void cntrl_ucm_init();
void cntlr_reg_domain_init(void);
#ifdef  CNTRL_CBENCH_SUPPORT
int32_t cbech_app_init();
#endif
/*FUNCTION_PROTOTYPE_END   *************************************************/
#endif /*__CONTROLLER_H*/
