#include "controller.h"
#include "cntlr_epoll.h"
#include "cntrl_queue.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "of_utilities.h"
#include "of_msgapi.h"
#include "cntlr_event.h"
#include <execinfo.h>
#include <sched.h>
#include "tmrldef.h"
#include "time.h"
#include "sll.h"
#include "memapi.h"
#include "memldef.h"
#include "dprmldef.h"
#include <sys/time.h>
#include "cntlr_sample_app.h"

of_msg_q_box_t  *app_msg_q_box;
void *of_app_thrd_entry(void *pArg);
void of_app_thrd_event_handler(int32_t fd,
				epoll_user_data_t    *info,
				uint32_t received_events);

void cntlr_app_handle_msgs(int32_t fd,
			epoll_user_data_t *info,
			uint32_t received_events);

/**** External Proto Types *****/
extern void sample_app_init(void);
extern int32_t	sm_app_igmp_init(void);
extern int32_t sm_arp_app_init(void);
extern void thread_loop();
extern of_msg_q_box_t* of_MsgQBoxCreate(uint32_t ulNoOfListNodesInQ, uint32_t num_queues);

#ifdef CNTRL_APP_THREAD_SUPPORT
void cntlr_shared_lib_init(void)
{
  int32_t ii;
  cntlr_thread_info_t       *thread_info;
  char                      acThNameStart[] = "OFAppThrd";

  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:%d Entered \n\r", __FUNCTION__,__LINE__);


  thread_info = (cntlr_thread_info_t* )of_mem_calloc(1,sizeof(cntlr_thread_info_t));
  if( thread_info == NULL)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:%d Error in creating Layer Thread #%d \n\r",
        __FUNCTION__,__LINE__,ii);
    exit(0);
  }
  thread_info->thread_id = 0;
  sprintf (thread_info->name, "%s%d", acThNameStart, ii);

  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Thread Id %d name %s",thread_info->thread_id,thread_info->name);


  of_spawn_thread(thread_info->name,
      &thread_info->posix_thread_id,
      of_app_thrd_entry,
      thread_info);

}
#else
void cntlr_shared_lib_init(void)
{
    sample_app_init();
    sm_arp_app_init();
    sm_app_igmp_init();
    sample_l2_init();
}
#endif

 void*
of_app_thrd_entry(void *pArg)
{
  cntlr_thread_info_t       *thread_info;
  epoll_user_data_t          user_data;
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

    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:%d thread id %d\r\n",__FUNCTION__,__LINE__, thread_info->thread_id);
    /*TBD of proper def at proper place*/
    retval = cntlr_epoll_init(NO_FD_SUPPORTED);
    if( retval != OF_SUCCESS )
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d  error 0x%x in initializing the ePoll library\r\n",__FUNCTION__,__LINE__, retval);
      status = OF_FAILURE;
      break;
    }

    /* Create loopback socket, bind to a port */
    thread_info->loopback_port    = 0;
    thread_info->loopback_sock_fd = 0;
    retval = of_get_ephermal_port(OF_IPPROTO_UDP,
        OF_LOOPBACK_SOCKET_IPADDR,
        &dyn_sock_handle,
        &dyn_loopback_port);
    if( retval != OF_SUCCESS )
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d error 0x%x in getting an ephermal loopback port\r\n",
          __FUNCTION__,__LINE__,
          retval);
      pthread_exit(NULL);
    }
    thread_info->loopback_port    = dyn_loopback_port;
    thread_info->loopback_sock_fd = dyn_sock_handle;

    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:%d  loopback_port = %d  \n",__FUNCTION__,__LINE__,dyn_loopback_port);
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:%d loopback_sock_fd = 0x%x \n",__FUNCTION__,__LINE__,dyn_sock_handle);


    app_msg_q_box = of_MsgQBoxCreate (OF_MAX_MSGS_IN_CHN_Q_LIST,
			(cntlr_info.number_of_threads+1));
    if(app_msg_q_box == NULL)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d Unable to create Message QBox for the channel \r\n", __FUNCTION__,__LINE__);
      status = OF_FAILURE;
      break;
    }

    memset(&user_data,0,sizeof(epoll_user_data_t));
    user_data.arg1 = (void *)app_msg_q_box;
    user_data.arg2 = (void *)thread_info;
   // OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"\r\n %s:%d  app_msg_q_box %p thread_info %p ", __FUNCTION__,__LINE__,app_msg_q_box, thread_info);
    interested_events = ePOLL_POLLIN;
    retval = cntlr_epoll_add_fd(
        app_msg_q_box->iLoopbackSockFd,
        interested_events,
        &user_data,
        cntlr_app_handle_msgs);
    if(retval == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d Error in adding Channel() Socket to EPOLL\r\n",
          __FUNCTION__,__LINE__);
      status = OF_FAILURE;
      break;
    }

    sample_app_init();


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


void of_app_thrd_event_handler(int32_t   fd,
		epoll_user_data_t    *info,uint32_t              received_events)
{
	cntlr_thread_info_t *thread_info = (cntlr_thread_info_t*)info->arg1;

	OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:%d Not implemented Event Handler %d \r\n",
			__FUNCTION__,__LINE__,thread_info->thread_id);
}


void cntlr_app_handle_msgs(int32_t   fd,
		epoll_user_data_t *info, uint32_t  received_events)
{
	struct sockaddr_in           remote_addr;
	uint32_t                     addr_length;
	struct of_app_msg               *msg        = NULL;
	cntlr_thread_comm_info_t     msg_event;
	int32_t                      retval     = OF_SUCCESS;
	of_msg_q_box_t              *app_msg_q_box_local=NULL;
	cntlr_thread_info_t       *thread_info=NULL;
	uint8_t                   try_lock_less_b = TRUE;

	if(info == NULL)
		return;

	OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"%s:%d entered\n", __FUNCTION__,__LINE__);
	app_msg_q_box_local   = (of_msg_q_box_t  *)info->arg1;
	thread_info = (cntlr_thread_info_t*)info->arg2;

	addr_length = sizeof(remote_addr);
	retval = recvfrom(fd,
			(char*)&msg_event,
			sizeof(cntlr_thread_comm_info_t),
			0,
			(struct sockaddr *)&remote_addr,
			(socklen_t *)&addr_length);
	if(retval < 0)
	{
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d error in reading the data on controller msg FD\n", __FUNCTION__,__LINE__);
		perror("read");
		return;
	}

	//  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"\r\n %s:%d recieved from thread %d app_msg_q_box_local %p app_msg_q_box %p thread_info %p", __FUNCTION__,__LINE__,
	//    msg_event.msg.sender_thread_id,app_msg_q_box_local,app_msg_q_box, thread_info);
	if(CNTLR_LIKELY(msg_event.command == OF_APP_MSG_RCVD))
	{
//		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:%d command is OF_APP_MSG_RCVD\n", __FUNCTION__,__LINE__);
		CNTLR_RCU_READ_LOCK_TAKE();

		if (msg_event.msg.sender_thread_id == 0)
			try_lock_less_b = FALSE;
		while (!of_MsgDequeue (app_msg_q_box_local,
				msg_event.msg.sender_thread_id, (void**)&msg,
				try_lock_less_b))
		{
//			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:%d *** table id %d msg %p\r\n", __FUNCTION__,__LINE__,msg->table_id,msg);
			switch(msg->table_id)
			{

				case  SM_APP_SESSION_TABLE_ID: 
//					OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"\r\n%s(%d):: session table" ,__FUNCTION__,__LINE__);
					sm_app_sess_table_pkt_handler(msg);
					break;
				case SM_APP_ETHTYPE_TABLE_ID:
//					OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"\r\n%s(%d):: ARP  table" ,__FUNCTION__,__LINE__);
					sm_app_arp_request_n_response_pkt_process(msg);
					break;
				case SM_APP_ARP_TABLE_ID:
//					OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"\r\n%s(%d):: ARP  table" ,__FUNCTION__,__LINE__);
					sm_app_arp_table_pkt_process(msg);
					break;
				case SM_APP_L3ROUTING_TABLE_ID:
//					OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"\r\n%s(%d):: L3 routin  table" ,__FUNCTION__,__LINE__);
					break;
				default:
//					OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"\r\n%s(%d):: unknown table id" ,__FUNCTION__,__LINE__);
					break;
			}
			if(msg != NULL)
				free(msg);
		}
		app_msg_q_box_local->bSendEvent=TRUE;
//		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"\r\n%s:%d setting event flag to true\n", __FUNCTION__,__LINE__);
		CNTLR_RCU_READ_LOCK_RELEASE();
	}
}


int32_t
cntlr_enqueue_msg_2_appxprt_conn(struct of_app_msg *msg)
{
  int32_t                     retval = OF_SUCCESS;
  int32_t                     status = OF_SUCCESS;
  uint8_t                     try_lock_less_b = TRUE;

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
  //  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"\r\n%s:%d ...app_msg_q_box %p msg %p\r\n", __FUNCTION__,__LINE__,app_msg_q_box, msg);
    if (cntlr_thread_index == 0)
        try_lock_less_b = FALSE;
    if(of_MsgEnqueue(app_msg_q_box, cntlr_thread_index, (void*)msg, try_lock_less_b) == OF_FAILURE   )
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d  Message enequeu to QBox failed \r\n", __FUNCTION__,__LINE__);
      status = OF_FAILURE;
      break;
    }
    if(app_msg_q_box->bSendEvent)
    {
      int32_t                     socket;
      cntlr_thread_comm_info_t    msg_event;
      struct sockaddr_in          peer_address;

    //  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:%d sending data \r\n", __FUNCTION__,__LINE__);
      socket = of_socket_create(OF_IPPROTO_UDP);
      if(socket == OF_FAILURE)
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d Socket creation error \r\n", __FUNCTION__,__LINE__);
        perror("socket");
        status = OF_FAILURE;
        break;
      }

      msg_event.command              = OF_APP_MSG_RCVD;
      msg_event.msg.sender_thread_id = cntlr_thread_index;

      peer_address.sin_family      = AF_INET;
      peer_address.sin_addr.s_addr = htonl(OF_LOOPBACK_SOCKET_IPADDR);
      peer_address.sin_port        = htons(app_msg_q_box->usLoopbackPort);

      //OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"\r\n %s:%d  Event send flag to false !!!\r\n",__FUNCTION__,__LINE__);
      app_msg_q_box->bSendEvent = FALSE;
      retval = sendto(socket,
          (char *)&msg_event,
          sizeof(cntlr_thread_comm_info_t),
          0,
          (struct sockaddr *)&peer_address,
          sizeof(peer_address));
      if( retval <= 0 )
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d  Event send to application thread failed !!!\r\n",__FUNCTION__,__LINE__);
        close(socket);
        status = OF_FAILURE;
        break;
      }
      close(socket);
    }
  }
  while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}
