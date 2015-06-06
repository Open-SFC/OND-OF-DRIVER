

/* * * * * ** * * * * * * * * G l o b a l * * * * * * * * * * * * * * * * * * */
/******************************************************************************
* * * * * * * * * * S T R U C T U R E   D E F I N I T I O N S * * * * * * * * * 
******************************************************************************/

typedef struct of_transprt_proto_info_s
{   
   uint8_t                 id;
   uint16_t                 proto; /** Port number of transport listening*/
   uint16_t                 port_number; /** Port number of transport listening*/
   uint8_t                 fd_type; /** Port number of transport listening*/
   char               dev_name[32];
   uint8_t                 path_name[108];
   of_transport_type_e      transport_type; /** Transport type TCP/TLS/UDP/DTLS*/
} of_trnsprt_proto_info_t;                    

typedef struct of_transport_proto_properties_s
{
   int32_t                 fd;
   uint16_t                proto; /** Port number of transport listening*/
   uint8_t                 fd_type;
   of_transport_type_e      transport_type; /** Transport type TCP/TLS/UDP/DTLS*/
   void *proto_spec;
}of_transport_proto_properties_t;


/******************************************************************************
* * * * * * * * * * * * Registration Function Prototypes * * * * * * * * * * *
******************************************************************************/
typedef int32_t (*of_proto_transport_init_p) (of_transport_proto_properties_t *);

typedef int32_t (*of_proto_transport_open_p) (int32_t fd, of_conn_properties_t *conn_details_p);

typedef int32_t (*of_proto_transport_write_data_p) (int32_t fd,
      void *conn_spec_info,
      void             *msg,
      uint16_t               length,
      int32_t flags,
      uint16_t               *len_write,
      uint8_t              *peer_closed_con);

typedef int32_t (*of_proto_transport_read_data_p) (int32_t fd,
      void **conn_spec_info,
      void             *msg,
      uint16_t               length,
      int32_t flags,
      uint16_t               *len_read,
      uint8_t              *peer_closed_con,
      void **opaque_pp);
typedef int32_t (*of_proto_transport_close_p) (int32_t fd,   void *conn_spec_info);

typedef int32_t (*of_proto_transport_deinit_p) (int32_t fd);

typedef int32_t (*of_proto_transport_get_hash_value_p) (void *conn_spec_info, int32_t *hash_p);

typedef int32_t (*of_proto_transport_compare_conn_spec_info_p) (void *conn_spec_info_one,void *conn_spec_info_two, int8_t *hit_p);

typedef int32_t (*of_proto_transport_compare_proto_spec_info_p) (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void *proto_spec , int8_t *hit_p);

typedef int32_t (*of_proto_transport_alloc_and_set_proto_spec_info_p) (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void **proto_spec);

typedef int32_t (*of_proto_transport_get_proto_spec_info_p) (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void *proto_spec);


struct of_transprt_proto_callbacks
{
   of_proto_transport_init_p  transport_init;
   of_proto_transport_open_p transport_open;
   of_proto_transport_write_data_p transport_write;
   of_proto_transport_read_data_p transport_read;
   of_proto_transport_close_p transport_close;
   of_proto_transport_deinit_p transport_deinit;
   of_proto_transport_get_hash_value_p transport_get_hash_value;
   of_proto_transport_compare_conn_spec_info_p transport_compare_conn_spec_info;
   of_proto_transport_compare_proto_spec_info_p transport_compare_proto_spec_info;
   of_proto_transport_alloc_and_set_proto_spec_info_p transport_alloc_and_set_proto_spec_info;
   of_proto_transport_get_proto_spec_info_p transport_get_proto_spec_info;
};

/******************************************************************************
 * * * * * * * * * * * * Function Prototypes * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t cntlr_register_transport_proto(of_trnsprt_proto_info_t  *trnsprt_proto_info_p,
      struct of_transprt_proto_callbacks *proto_callbacks_p);

int32_t cntlr_add_transport_proto( of_trnsprt_proto_info_t *trnsprt_proto_info_p);

int32_t cntlr_get_first_transport_proto( of_trnsprt_proto_info_t *trnsprt_proto_info_p);

int32_t cntlr_get_next_transport_proto( of_trnsprt_proto_info_t *trnsprt_proto_info_p);

int32_t cntlr_get_exact_transport_proto( of_trnsprt_proto_info_t *trnsprt_proto_info_p);

int32_t cntlr_delete_transport_proto( of_trnsprt_proto_info_t *trnsprt_proto_info_p);

int32_t cntlr_deregister_transport_proto(of_trnsprt_proto_info_t  *trnsprt_proto_info_p);

int32_t cntlr_delete_transport_by_proto( int8_t   transport_type);


/* * * * * ** * * * * * * * * L o c a l * * * * * * * * * * * * * * * * * * */
/******************************************************************************
*********************  Macro Definitions *************************************
******************************************************************************/
#define CNTLR_TCP_ACCEPT_Q_LEN  (DPRM_MAX_DATAPATH_ENTRIES *\
                OF_MAX_NO_OF_CHANNELS_WITHIN_DP) 


#define OF_TRANSPORT_PROTO_FD_TYPE_SHARED 1
#define OF_TRANSPORT_PROTO_FD_TYPE_DEDICATED 2
#define OF_TRANSPORT_PROTO_FD_TYPE_SHARED_AND_DEDICATED 3

#define XPRT_PROTO_MAX_STRING_LEN 128
#define OF_IPPROTO_NONE 0

#define COMP_MATCH 1
#define COMP_NO_MATCH 0

/******************************************************************************
*********************  TRANSPORT FSM ******************************************
******************************************************************************/
typedef int32_t (*of_xprt_proto_fsm_function)(cntlr_trnsprt_info_t *,  void **);
typedef enum
{
   OF_XPRT_PROTO_STATE_INIT=0,
   OF_XPRT_PROTO_STATE_READY_FOR_CONN,
   OF_XPRT_PROTO_STATE_CLOSE_TRANSPORT,
   OF_XPRT_PROTO_STATE_MAX
}xprt_proto_state_e;


typedef enum
{
   OF_XPRT_PROTO_LEARN_PROTO_EVENT=0,
   OF_XPRT_PROTO_NEW_CONN_EVENT,
   OF_XPRT_PROTO_READ_EVENT,
   OF_XPRT_PROTO_WRITE_EVENT,
   OF_XPRT_PROTO_READ_FAIL_EVENT,
   OF_XPRT_PROTO_WRITE_FAIL_EVENT,
   OF_XPRT_PROTO_ERROR_EVENT,
   OF_XPRT_PROTO_CLOSE_EVENT,
   OF_XPRT_PROTO_DEL_PROTO_EVENT,
   OF_XPRT_PROTO_EVENT_MAX,
}xprt_proto_event_e;

/******************************************************************************
*********************  CHANNEL FSM ******************************************
******************************************************************************/
typedef int32_t (*of_xprt_proto_channel_fsm_function)( cntlr_channel_info_t *, void *);
void fsm_send_channel_event(cntlr_channel_info_t *channel,int8_t fsm_event);
int32_t fsm_xprt_proto_channel_write_msg_to_dp(cntlr_channel_info_t *channel,
      struct of_msg           *msg,
      uint16_t                len_already_sent,
      uint16_t                *len_sent_currently);
int32_t fsm_xprt_proto_channel_read_msg_from_dp( cntlr_channel_info_t *channel,
      uint8_t               *msg,
      uint16_t               length,
      uint16_t               *len_read);

typedef enum
{
   OF_XPRT_PROTO_CHANNEL_STATE_INIT=0,
   OF_XPRT_PROTO_CHANNEL_STATE_WAIT_FOR_HANDSHAKE,
   OF_XPRT_PROTO_CHANNEL_STATE_BOUND_TO_DP,
   OF_XPRT_PROTO_CHANNEL_STATE_CLOSE_CONN,
   OF_XPRT_PROTO_CHANNEL_STATE_MAX
}xprt_proto_channel_state_e;

typedef enum
{
   OF_XPRT_PROTO_CHANNEL_LEARN_EVENT=0,
   OF_XPRT_PROTO_CHANNEL_DP_ATTACH_EVENT=0,
   OF_XPRT_PROTO_CHANNEL_DP_DETACH_EVENT,
   OF_XPRT_PROTO_CHANNEL_READ_EVENT,
   OF_XPRT_PROTO_CHANNEL_WRITE_EVENT,
   OF_XPRT_PROTO_CHANNEL_READ_FAIL_EVENT,
   OF_XPRT_PROTO_CHANNEL_WRITE_FAIL_EVENT,
   OF_XPRT_PROTO_CHANNEL_ERROR_EVENT,
   OF_XPRT_PROTO_CHANNEL_CLOSE_EVENT,
   OF_XPRT_PROTO_CHANNEL_EVENT_MAX,
}xprt_proto_channel_event_e;

#define CNTLR_XPRT_PROTO_PRINT_STATE(trnport_info) \
{\
    OF_LOG_MSG (OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"trsnprt proto=%s state=%s event=%s",\
 cntlr_xprt_protocols[trnsprt_info->transport_type],\
 cnlr_xprt_fsm_states[trnport_info->fsm_state],\
cnlr_xprt_fsm_events[trnport_info->fsm_event] );\
}

#define CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE(channel) \
{\
      OF_LOG_MSG (OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "channel id=%d proto=%s state=%s event=%s",  \
	channel->id,\
	cntlr_xprt_protocols[channel->trnsprt_info->transport_type ],\
	cnlr_xprt_proto_channel_states[channel->fsm_state],\
	cnlr_xprt_proto_channel_events[channel->fsm_event] );\
}

#define CNTLR_XPRT_CHANNEL_ALLOC(channel, is_heap) \
{\ 
   if(mempool_get_mem_block(cntlr_trnsprt_channel_mempool_g,\
            (unsigned char **)&channel,\
            (unsigned char *)&is_heap) != OF_SUCCESS)\
   {\
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Channel memory alloc failure");\
      status = OF_FAILURE;\
      break;\
   }\
   memset(channel,0,sizeof(cntlr_channel_info_t));\
   channel->is_heap=is_heap;\
}

#define CNTLR_XPRT_MAX_CHANNEL_ID 10000 

#define CNTLR_XPRT_GENERATE_CHANNEL_ID(id) \
{\
   cntlr_xprt_channel_id_g++;\
   if ( cntlr_xprt_channel_id_g == CNTLR_XPRT_MAX_CHANNEL_ID )\
   cntlr_xprt_channel_id_g=0;\
   id= cntlr_xprt_channel_id_g;\
}  

#define CNTLR_XPRT_MAX_PROTOCOL_ID 255
#define CNTLR_XPRT_GENERATE_PROTOCOL_ID(id) \
{\
   cntlr_xprt_protocol_id_g++;\
   if ( cntlr_xprt_protocol_id_g == CNTLR_XPRT_MAX_PROTOCOL_ID )\
   cntlr_xprt_protocol_id_g=0;\
   id= cntlr_xprt_protocol_id_g;\
   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"New protocol id %d",id);\
}  

#define CNTLR_XPRT_GET_PROTO_NAME(id, buf) \
{\
   if (id ==OF_IPPROTO_TCP )\
   {	\
	strcpy(buf,"TCP");\
   }\
   else if (id ==OF_IPPROTO_UDP )\
   {	\
	strcpy(buf,"TCP");\
   }\
   else\
	strcpy(buf,"NONE");\
}

#define CNTLR_XPRT_GET_PROTO_TRANSPORT_TYPE( buf,type) \
{\
  int32_t i;\
	for (i=0; i <CNTLR_TRANSPORT_TYPE_MAX ; i++)\
	{\
		if (!strcmp(cntlr_xprt_protocols[i],buf))\
		{\
			type=i;\
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"proto %s type %d",buf, type);\
			break;\
		}\
	}\
}

#define CNTLR_XPRT_PRINT_PKT_DATA(pkt, length) \
{\
	int32_t i, j;\
	char data[2048];\
	for (i =0, j=0; i < length; i++, j=j+3)\
	{\
		if (!(i%16))\
		{\
			sprintf(&data[j],"\n");\
			j++;\
		}\
		if (!(i%8))\
		{\
			sprintf(&data[j],"   ");\
			j++;\
		}\
		sprintf(&data[j],"%02x  ",*(pkt + i ));\
	}\
OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"pkt data %s",data);\
}

#define CNTLR_XPRT_PRINT_CHANNEL_MSG_HEADER(channel) \
{\
OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Msg version %x type %x length %u xid %d",\
channel->msg_hdr.version,\
channel->msg_hdr.type,\
ntohs(channel->msg_hdr.length),\
ntohl(channel->msg_hdr.xid)\
);\
}

#define CNTLR_XPRT_PROTO_SET_TRNSPRT_INFO_FOR_CHANNEL(channel,trnsprt_info)   channel->trnsprt_info=trnsprt_info
#define CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_STATE(channel,state)   channel->fsm_state=state
#define CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_EVENT(channel,event)   channel->fsm_event=event

#define CNTLR_XPRT_PROTO_SET_TRANSPORT_FSM_STATE(trnsprt_info,state)   trnsprt_info->fsm_state=state
#define CNTLR_XPRT_PROTO_SET_TRANSPORT_FSM_EVENT(trnsprt_info,event)   trnsprt_info->fsm_event=event

/******************************************************************************
 * * * * * * * * * * * Poll Function Prototypes * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
   void
cntlr_listen_for_new_connections( int32_t   fd,
      epoll_user_data_t *user_data,
      uint32_t           received_events);

   void
cntlr_listen_for_dp_msgs_for_shared_fd( int32_t   fd,
      epoll_user_data_t *user_data,
      uint32_t           received_events);

void
cntlr_trnsprt_handle_handshake(int32_t            fd,
    epoll_user_data_t  *info,
    uint32_t            received_events);


void
cntlr_trnsprt_handle_ctrl_thread_msgs(int32_t            fd,
    epoll_user_data_t *info,
    uint32_t           received_events);


void
of_handle_dp_msg(int32_t fd,
      epoll_user_data_t *info,
      uint32_t received_events);

/******************************************************************************
 * * * * * * * * * * * Channel Function Prototypes * * * * * * * * * * * * * * * 
 *******************************************************************************/
int32_t
cntlr_trnsprt_create_channel( 
      uint8_t                 trnsprt_type,
      int32_t                 trnsprt_fd,
      ippaddr_t               dp_ip,
      uint16_t                dp_port_no,
      cntlr_channel_info_t  **p_p_channel);

int32_t
cntlr_trnsprt_channel_init( 
      cntlr_channel_info_t  *channel_p,
      uint8_t                 trnsprt_type,
      int32_t                 trnsprt_fd,
      void                  *conn_spec_info   
      );

int32_t
of_add_channel_to_dp( cntlr_conn_table_t          *conn_table,
                      cntlr_conn_node_saferef_t   *conn_safe_ref,
                      struct ofp_switch_features  *sw_feature_msg,
                      struct dprm_datapath_entry  **datapath_p,
                      uint64_t                    *dp_handle_p);

void cntlr_trnsprt_conn_close(cntlr_channel_info_t   *channel);

int32_t cntlr_trnsprt_mchash_table_init(cntlr_trnsprt_info_t *trnsprt_info);

int32_t channel_socket_recv_msgs_init(cntlr_channel_info_t *channel);

/******************************************************************************
 * * * * * * * * * * Channel FSM Function Prototypes * * * * * * * * * * * * *
 *******************************************************************************/
void fsm_update_channel_state(cntlr_channel_info_t *channel,int8_t fsm_state);

/******************************************************************************
 * * * * * * * * * * Channel Msgs Function Prototypes * * * * * * * * * * * * *
 *******************************************************************************/
int32_t
cntlr_trnsprt_send_handshake_msg(cntlr_channel_info_t  *channel);

int32_t
cntlr_trnsprt_send_hello_msg(cntlr_channel_info_t  *channel);

/******************************************************************************
 * * * * * * * * * * Channel Core Function Prototypes * * * * * * * * * * * * *
 *******************************************************************************/
inline void 
of_send_all_msgs_waiting_on_chn_to_dp(cntlr_channel_info_t *channel);


int32_t
cntlr_tnsprt_try_2_send_msg_on_line(cntlr_channel_info_t *channel,
      struct of_msg *msg,
      uint8_t  is_new_msg,
      uint16_t msg_offset,
      uint16_t *len_sent_currently);

inline int32_t 
cntlr_tnsprt_send_msg_to_dp_channel(cntlr_channel_info_t *channel,
      struct of_msg *msg,
      uint8_t  is_new_msg,
      uint16_t msg_offset,
      uint16_t *len_sent_currently);

void
of_add_2_epoll_4_wr_event(cntlr_channel_info_t *channel,
      struct of_msg        *msg,
      uint8_t              is_new_msg,
      uint16_t             len_sent_currently);

inline int32_t
cntlr_transprt_send_to_dp_locally(struct of_msg              *msg,
      cntlr_conn_table_t         *conn_table,
      cntlr_conn_node_saferef_t  *conn_safe_ref);
