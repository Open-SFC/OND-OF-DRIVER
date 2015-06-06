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

 /* File: cntlr_epoll.h
 * 
 * Description: Definitions of epoll library
 * 
 * Authors:     Rajesh Madabushi <rajesh.madabushi@freescale.com>
 * Modifier:    
 **/
 /*
 **  History
 *   February, 2013 Rajesh Madabushi - Initial Implementation.
 *
 */

/** Applicatoin can define arguments passed to handler  
 ** Application can define two arguments, it will be passed to the handler
 ** whenever events received on the FD, ealier this FD was added to the 
 ** epoll context of the thread.
 **/
typedef  struct  epoll_user_data_s
{
  void*  arg1; /** Argumnet passed to handler to receive events*/
  void*  arg2; /** Argument passed to handler to receive events*/
  void*  arg3; /** Argument passed to handler to receive events*/
  unsigned int index;
  unsigned int magic;
}epoll_user_data_t;

/** Fd(file descriptor) handler function prototo typen
 */
typedef  void  (*epoll_fd_handler_t) (int  fd,
                                      epoll_user_data_t*  user_data, 
                                      unsigned int received_events);

/** enum values to know whether the  FD is currently active or not
 */
typedef enum
{
    ePOLL_FD_ACTIVE  = 0x01,
    ePOLL_FD_DELETED = 0x02
}epoll_fd_flag_t;

typedef  enum
{
  ePOLL_POLLIN    = 0x01,  /* reading */
  ePOLL_POLLPRI   = 0x02,  /* urgent data is ready for reading */
  ePOLL_POLLOUT   = 0x04,  /* writing */
  ePOLL_POLLERR   = 0x08,  /* valid only in receive events */
  ePOLL_POLLHUP   = 0x10,  /* valid only in receive events */
  ePOLL_POLLINVAL = 0x20,  /* invalid FD request. FD not open */
                               /* valid only in receive events */
} epoll_events_t;

#define EPOLL_MAX_EVENTS 20 /*TBD tuning */

/** Details of each FD that added epoll context. It contains FD, function handler
 *  to callled once receiving events and argument passing to the handler
 */
typedef struct epoll_fd_s
{
    unsigned char      flag; /** Flags indicatint particual epoll FD is active ot not*/
    int                fd;   /** OS FD pushed to epoll context*/
    unsigned int       intrested_events;  /* events interested to receive on the FD*/
    epoll_user_data_t  user_data; /** User data that need to pass as an argument to epoll handler*/
    epoll_fd_handler_t user_handler; /** epoll handler to be called when interesteed events received on the FD*/
}epoll_fd_t;

/** epollFD list: List of FD details as explained in the above data-structure*/
typedef struct epoll_fd_list_s
{
    struct epoll_fd_list_s *next;
    unsigned int          index; /** possition in the list where FD stored*/
    epoll_fd_t            *epoll_fd; /** pointer to epoll FD details */
}epoll_fd_list_t;

/** Details of epoll context attached to thread. It contains max num of fds,
 * current active fds, list of FDs, free list to know next available locaton FD list
 */
typedef  struct  epoll_cntxt_s
{
    int              epoll_dev_fd;   /** /dev/epoll" file descriptor for this context */
    unsigned int     max_no_of_fds;  /** Maximum fds used in this context */
    epoll_fd_list_t *fd_list;        /** Array of fds being polled currently */
    epoll_fd_list_t *fd_free_list;   /** List of fd slot that are free */
    epoll_fd_list_t *fd_free_list_tail;/** pointer to tail node node of free list*/
    struct epoll_event*  events;     /** Memory for max_fds. used with epoll_wait()*/
}epoll_cntxt_t;
/* DEFINE_START ***********************************************************************/
#define cntlr_epoll_add_fd(fd,intrested_events,user_data,user_handler)\
           cntlr_epoll_add_fd_to_thread(self_thread_info,fd,intrested_events,user_data,user_handler)

#define cntlr_epoll_modify_fd(fd,intrested_events,user_data,user_handler)\
           cntlr_epoll_mod_fd_of_thread(self_thread_info,fd,intrested_events,user_data,user_handler)

#define cntlr_epoll_del_fd(fd) cntlr_epoll_del_fd_from_thread(self_thread_info,fd)
/* DEFINE_END   ***********************************************************************/

/*FUNCTION PROTOTYPES_START *********************************************************/
int32_t cntlr_epoll_init(uint32_t   max_fds);
void cntlr_epoll_shutdown();
int32_t
cntlr_epoll_add_fd_to_thread(cntlr_thread_info_t  *thread_info,
                       int32_t               fd,
                       uint32_t              intrested_events,
                       epoll_user_data_t    *user_data,
                       epoll_fd_handler_t    user_handler);
int32_t
cntlr_epoll_mod_fd_of_thread(cntlr_thread_info_t  *thread_info,
                             int32_t               fd,
                             uint32_t              intrested_events,
                             epoll_user_data_t    *user_data,
                             epoll_fd_handler_t    user_handler);

void
cntlr_epoll_del_fd_from_thread(cntlr_thread_info_t  *thread_info,
                         int32_t               fd);
void cntlr_epoll_wait(uint32_t   wait_time);
/*FUNCTION PROTOTYPES_END   *********************************************************/

