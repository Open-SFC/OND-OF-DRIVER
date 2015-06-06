/** 
 ** Copyright (c) 2012, 2013  Freescale.
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

 /* File: of_epoll.c
 *  
 *  Description:  epoll library APIs
 *  
 * Authors:     Rajesh Madabushi <rajesh.madabushi@freescale.com>
 * History
 *   February, 2013 Rajesh Madabushi - Initial Implementation.
 * 
 */

/**
 * Initialize epoll context for the current tead
 * \param [in] max_fd        - Maximum number of file descriptors opened
 */

#include "controller.h"
#include <sys/epoll.h>
#include "cntlr_epoll.h"
#include "oflog.h"
int32_t
cntlr_epoll_init(uint32_t   max_fds)
{
    epoll_cntxt_t *epoll_cntxt = NULL;
    int32_t  status            = OF_SUCCESS;

    do
    {
       epoll_cntxt = 
       self_thread_info->p_epoll_lib_cntxt = (epoll_cntxt_t *) calloc(1, sizeof(epoll_cntxt_t));
       if(epoll_cntxt == NULL)
       {
           perror("Thread epoll cntxt memory alloc error");
           status = OF_FAILURE;
           break;
       }

       epoll_cntxt->epoll_dev_fd = epoll_create(max_fds);
       if(epoll_cntxt->epoll_dev_fd == -1 )
       {
           perror("epoll_create");
           status = OF_FAILURE;
           break;
       }
       epoll_cntxt->max_no_of_fds = max_fds; 

       epoll_cntxt->events = (struct epoll_event*)calloc( (max_fds+1),
                                                         sizeof(struct epoll_event) );
       if( epoll_cntxt->events == NULL )
       {
           perror("epoll cntxt event mem alloc error");
           status = OF_FAILURE;
           break;
       } 

       epoll_cntxt->fd_list = (epoll_fd_list_t*) calloc( (max_fds+1),
                                                       sizeof(epoll_fd_list_t) );
       if( epoll_cntxt->fd_list == NULL )
       {
           perror("epoll cntxt fd list mem alloc error");
           status = OF_FAILURE;
           break;
       }

        /** Build FD free list **/
       {
        epoll_fd_list_t *temp;
        uint32_t         temp_index;

        temp_index = max_fds;
        epoll_cntxt->fd_free_list_tail = 
        epoll_cntxt->fd_free_list = &epoll_cntxt->fd_list[temp_index];
        epoll_cntxt->fd_free_list->index = temp_index;
        while(temp_index > 1)
        {
            temp_index--;
            temp        = epoll_cntxt->fd_list + temp_index;
            temp->index = temp_index;
            temp->next  = epoll_cntxt->fd_free_list;
            epoll_cntxt->fd_free_list = temp;
        }
       }
    }
    while(0);

    if(status == OF_FAILURE)
    {
       if(epoll_cntxt != NULL)
       {
          if(epoll_cntxt->events != NULL)
             free(epoll_cntxt->events);

          if(epoll_cntxt->fd_list != NULL)
             free(epoll_cntxt->fd_list);

          if(epoll_cntxt->epoll_dev_fd > 0)
             close(epoll_cntxt->epoll_dev_fd);

          free(epoll_cntxt);
       }
       return status;
    }

    return OF_SUCCESS;
}

void
cntlr_epoll_shutdown()
{
   epoll_cntxt_t   *epoll_cntxt;

   epoll_cntxt      = self_thread_info->p_epoll_lib_cntxt;

   if(epoll_cntxt != NULL)
   {
      if(epoll_cntxt->events != NULL)
         free(epoll_cntxt->events);

      if(epoll_cntxt->fd_list != NULL)
         free(epoll_cntxt->fd_list);

      if(epoll_cntxt->epoll_dev_fd > 0)
         close(epoll_cntxt->epoll_dev_fd);

      free(epoll_cntxt);
  }
 
}

/* Adds FD to epoll context, register handle to receive events on the fd
 * \param [in] fd                - File descriptor that adding to epoll context
 * \param [in] interested_events - read or write events on which registering handler
 * \param [in] user_data         - Application passed private data that passed to handler
 * \param [in] user_handler      - Funcion handler to pass events
 */
int32_t
cntlr_epoll_add_fd_to_thread(cntlr_thread_info_t  *thread_info,
                       int32_t               fd,
                       uint32_t              intrested_events,
                       epoll_user_data_t    *user_data,
                       epoll_fd_handler_t    user_handler)
{
    uint32_t              fd_list_pos;
    epoll_fd_t           *epoll_fd=NULL;
    epoll_cntxt_t        *epoll_cntxt;
    struct epoll_event    event;
    int32_t               status = OF_SUCCESS; ;
    int32_t               retval = OF_SUCCESS; ;

 //   cntlr_assert(thread_info!=NULL);
     //cntlr_assert(thread_info);

    do
    {
        epoll_cntxt = thread_info->p_epoll_lib_cntxt;

        if(epoll_cntxt->fd_free_list == NULL)
        {
            OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_WARN,"%s:%d FD Free list is NULL\r\n",__FUNCTION__,__LINE__);
            status = OF_FAILURE;
            break;
        }
    
        /** Get free FD slot position in FD table.
         */
        fd_list_pos = epoll_cntxt->fd_free_list->index;

        /*Allocate memory for the ePollFd node  */
        epoll_fd = (epoll_fd_t*)calloc(1, sizeof(epoll_fd_t));
        if(epoll_fd == NULL)
        {
            perror("Malloc ePollFD");
            status = OF_FAILURE;
            break;
        }

        /*Remember FD avent details in the FD list of the epoll context;*/
        epoll_fd->flag            |= ePOLL_FD_ACTIVE;
        epoll_fd->fd               = fd;
        epoll_fd->intrested_events = (intrested_events | ePOLL_POLLERR | ePOLL_POLLHUP);
        epoll_fd->user_data.arg1   = user_data->arg1;
        epoll_fd->user_data.arg2   = user_data->arg2;
        epoll_fd->user_data.arg3   = user_data->arg3;
        epoll_fd->user_data.index  = user_data->index;
        epoll_fd->user_data.magic  = user_data->magic;
        epoll_fd->user_handler     = user_handler;

        epoll_cntxt->fd_list[fd_list_pos].epoll_fd = epoll_fd;

        /** Now add this fd to the linux epoll*/
        event.events   = (epoll_fd->intrested_events | ePOLL_POLLERR | ePOLL_POLLHUP);
        event.data.u32 = fd_list_pos;

        /** Add FD to epoll context **/
        retval = epoll_ctl( epoll_cntxt->epoll_dev_fd,
                            EPOLL_CTL_ADD,
                            fd,
                            &event);
        if(retval == -1)
        {
            perror("epoll_ctl");
            status = OF_FAILURE;
            epoll_cntxt->fd_list[fd_list_pos].epoll_fd = 0;
            break;
        }

        /** Update FD frelist pointer **/
        epoll_cntxt->fd_free_list= epoll_cntxt->fd_free_list->next;
    }
    while(0);

    if(status == OF_FAILURE)
    {
       if(epoll_fd != NULL)
         free(epoll_fd);
    }
   return status;
}

int32_t
cntlr_epoll_mod_fd_of_thread(cntlr_thread_info_t  *thread_info,
                             int32_t               fd,
                             uint32_t              intrested_events,
                             epoll_user_data_t    *user_data,
                             epoll_fd_handler_t    user_handler)
{
     int32_t              index;
     epoll_fd_t           *epoll_fd=NULL;
     epoll_cntxt_t       *epoll_cntxt;
     epoll_fd_list_t     *epoll_fd_list; 
     struct epoll_event   event;

     epoll_cntxt = thread_info->p_epoll_lib_cntxt;

     /*TBD , Modify is costly operation, find better method*/
     for(index = 1; index <=epoll_cntxt->max_no_of_fds;index++)
     {
         epoll_fd_list = (epoll_cntxt->fd_list + index);
         if( (epoll_fd_list->epoll_fd != NULL)  &&
             (epoll_fd_list->epoll_fd->fd == fd) )
         {

            epoll_fd = epoll_fd_list->epoll_fd;
            epoll_fd->intrested_events = (intrested_events | ePOLL_POLLERR | ePOLL_POLLHUP);
            epoll_fd->user_data.arg1   = user_data->arg1;
            epoll_fd->user_data.arg2   = user_data->arg2;
            epoll_fd->user_data.arg3   = user_data->arg3;
            epoll_fd->user_handler     = user_handler;

            event.events   = (epoll_fd->intrested_events | ePOLL_POLLERR | ePOLL_POLLHUP);
            event.data.u32 = index;

             if(epoll_ctl(epoll_cntxt->epoll_dev_fd,
                          EPOLL_CTL_MOD,
                          fd,
                          &event) != 0)
             {
                 OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"%s:%d error in deleting fd from epoll ctl\r\n",__FUNCTION__,__LINE__);
                 perror("epoll_ctl");
                 /*TBD revert back original status*/
                 return OF_FAILURE;
             }
             return OF_SUCCESS;
         }
     } 
     return OF_FAILURE;
}
void
cntlr_epoll_del_fd_from_thread(cntlr_thread_info_t  *thread_info,
                         int32_t               fd)
{
     int32_t              index;
     epoll_cntxt_t       *epoll_cntxt;
     epoll_fd_list_t     *epoll_fd_list; 
     struct epoll_event   event;

     epoll_cntxt = thread_info->p_epoll_lib_cntxt;

     for(index = 1; index <=epoll_cntxt->max_no_of_fds;index++)
     {
         epoll_fd_list = (epoll_cntxt->fd_list + index);
         if( (epoll_fd_list->epoll_fd != NULL)  &&
             (epoll_fd_list->epoll_fd->fd == fd) )
         {
             event.events = epoll_fd_list->epoll_fd->intrested_events;

             if(epoll_ctl(epoll_cntxt->epoll_dev_fd,
                          EPOLL_CTL_DEL,
                          fd,
                          &event) != 0)
             {
                 OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"%s:%d error in deleting fd from epoll ctl\r\n",__FUNCTION__,__LINE__);
                 perror("epoll_ctl");
                 return;
             }

             free( epoll_fd_list->epoll_fd);
             epoll_fd_list->epoll_fd = NULL;

             epoll_cntxt->fd_free_list_tail->next = epoll_fd_list; 
             epoll_cntxt->fd_free_list_tail       = epoll_fd_list;
             return;
         }
     } 
}

void 
cntlr_epoll_wait(uint32_t   wait_time)
{
   epoll_cntxt_t       *epoll_cntxt;
   struct epoll_event  *event;
   uint32_t             total_ready_fds;
   uint32_t             ii;
   epoll_fd_t           *epoll_fd;
   uint32_t             fd_list_pos;

resend:
   epoll_cntxt      = self_thread_info->p_epoll_lib_cntxt;

   total_ready_fds = epoll_wait(epoll_cntxt->epoll_dev_fd,
                                epoll_cntxt->events,
                                EPOLL_MAX_EVENTS,
                                wait_time);
   if( total_ready_fds != -1 )
   {
      for( ii = 0, event = epoll_cntxt->events;
                 (ii < total_ready_fds) && (event != NULL); ii++, event++ )
      {
         fd_list_pos = event->data.u32;

         if( CNTLR_UNLIKELY((fd_list_pos < 1)) || CNTLR_UNLIKELY((fd_list_pos > epoll_cntxt->max_no_of_fds)) )
         {
              OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"%s: %d epoll_wait returned invalid u32 %u\r\n", __FUNCTION__,__LINE__, fd_list_pos);
              continue;
         }
         epoll_fd = epoll_cntxt->fd_list[fd_list_pos].epoll_fd;
         if( CNTLR_UNLIKELY(epoll_fd == NULL ) )
         {
             OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"%s:%d epoll_wait no fd registered at pos %u\r\n", __FUNCTION__, __LINE__,fd_list_pos);
             continue;
         }
         if( CNTLR_UNLIKELY(epoll_fd->flag & ePOLL_FD_DELETED) || CNTLR_UNLIKELY(!(epoll_fd->flag & ePOLL_FD_ACTIVE)) )
         {
             OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"%s:%d epoll_wait deleted at pos %u\r\n", __FUNCTION__, __LINE__,fd_list_pos);
             /*
              * This fd might have deleted(closed) by the user
              */
             continue;
         }
         /* Call handler that registered to receive events on this FD*/
         if( epoll_fd->user_handler )
         {        
                epoll_fd->user_handler( epoll_fd->fd,
                                        &(epoll_fd->user_data),
                                        event->events );
         }
      }
   }
#if 1
   else if(errno == EINTR)	 
   {
	   OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_DEBUG, "%d Interrupt system call, resend epoll again", epoll_cntxt->epoll_dev_fd);
	   goto resend;
   }
#endif
   else 
   {
	   perror("epoll_wait");
	   OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR, " Interrupt system call, resend epoll again %d error %d", epoll_cntxt->epoll_dev_fd,errno);
   }
}
