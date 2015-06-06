/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
*/

/*
 *
 * File name: cmsock.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: 
 * 
 */

#ifndef _UCMSOCK_H_
#define _UCMSOCK_H_

typedef int cm_sock_handle;

/*
 * TCP/IP Protocol IDs :
 */
#define CM_IPPROTO_ANY   0
#define CM_IPPROTO_TCP   6
#define CM_IPPROTO_UDP   17
#define CM_IPPROTO_ICMP  1
#define CM_IPPROTO_IGMP  2
#define CM_IPPROTO_SCTP  132
#define CM_IPPROTO_IPV6  41
#define CM_IPPROTO_IPCOMP  108
#define CM_IPPROTO_ICMPV6 58
#define CM_IPV6_NOEXT_HDR 59
#define CM_IPV6_MOBILITY_HDR 135
#define CM_IPPROTO_OTHER 0
#ifdef OF_IPIP_SUPPORT
#define CM_IPPROTO_IPIP   4
#endif /* OF_IPIP_SUPPORT*/

#define CM_IPv6_ADDRU8_LEN 16
#define CM_IPv6_ADDRU16_LEN 8
#define CM_IPv6_ADDRU32_LEN 4
/*
 * FDs
 */

#define CM_FDPOLL_MAXFDS  32

#define CM_FDPOLL_ZERO(p, n) of_memset(p, 0, (n)*sizeof(*(p)))
#define CM_FDPOLL_READ  0x01
#define CM_FDPOLL_WRITE 0x02
#define CM_FDPOLL_EXCEPTION 0x04
#define CM_FDPOLL_URGENT 0x08
typedef enum
{
  cm_inet_ipv4=1,
  cm_inet_ipv6
} cm_ip_types_e;

struct cm_ipv6_addr
{
    union
    {
      uint8_t cm_ip_v6_addr_u8[CM_IPv6_ADDRU8_LEN]; 
      uint16_t cm_ip_v6_addr_u16 [CM_IPv6_ADDRU16_LEN];
      uint32_t cm_ip_v6_addr_u32 [CM_IPv6_ADDRU32_LEN];
    } Addr;
    #define ip_v6_addr_u8 Addr.cm_ip_v6_addr_u8 
    #define ip_v6_addr_u16 Addr.cm_ip_v6_addr_u16 
    #define ip_v6_addr_u32 Addr.cm_ip_v6_addr_u32 
};

struct cm_ip_addr
{
 uint32_t ip_type_ul;
  union
  {
     uint32_t     ip_addr_ul;
     struct cm_ipv6_addr ip_v6_addr;
  }IP;
#define ip_v6_addr_8 IP.ip_v6_addr.ip_v6_addr_u8
#define ip_v6_addr_16 IP.ip_v6_addr.ip_v6_addr_u16
#define ip_v6_addr_32 IP.ip_v6_addr.ip_v6_addr_u32
#define IPv4Addr IP.ip_addr_ul
};

struct cm_poll_info
{
  cm_sock_handle fd_i;
  uint32_t interested_events;
  uint32_t returned_events;
  void *user_data_p;
};

struct cm_time_out
{
#ifdef LINUX_SUPPORT
  struct timeval time_out;
#else
  uint32_t time_out_ui;              /* Count in ms */
#endif                          /* LINUX_SUPPORT */

};

/*
 * structure used for configuring the TCP keep alive option on the sockets
 * enable_b - indicates whether the TCP keep alive option should be enabled or
 *           or disabled on the given socket.
 * idle_time_ui - Keep Alive Time. number of seconds needs to be idle before TCP
 *              begins sending out keep alive probe pkts. The default value is 
 *              7200 seconds(or 2 hours). Valid only when keep alive option
 *              is enabled.
 * probe_interval_ui - number of seconds between TCP keep-alive probes(if there
 *                   is no response fro other end for the TCP keep alive pkts. 
 *                   Otherwise inactivity and ofcourse idle_time_ui will decide 
 *                   time when the next TCP keep alive is sent). Default
 *                   value is 75 seconds. Valid only when keep alive option
 *                   is enabled.
 * probe_count_ui - maximum number of TCP keep alive probes to be sent before
 *                giving up and killing the connection if no responses is 
 *                obtained from the other end. The default value is 9. Valid 
 *                only when keep alive option is enabled.
 * 
 * So time taken(in worst case) to detect the failed connections and close them
 * is (idle_time_ui + (probe_interval_ui * probe_count_ui)) seconds
 *   
 */
struct cm_tcp_keep_alive_options_info
{
  unsigned char enable_b;
  uint32_t idle_time_ui;          /* in seconds */
  uint32_t probe_interval_ui;     /* in seconds */
  uint32_t probe_count_ui;
};

struct cm_socket_linger_option_info
{
  unsigned char enable_b;
  uint32_t linger_time_ui;        /* time to linger in seconds */
};

typedef enum
{
  cm_socket_shutdown_read = 0,
  cm_socket_shutdown_write,
  cm_socket_shutdown_read_write,
} cm_socket_shutdown_how_opts_t;

#if defined(LINUX_SUPPORT)

//#include <sys/types.h>
//#include <sys/time.h>
//#include <unistd.h>

#define    cm_fd_set_t         fd_set
#define SET_RESET_BLOCK_MODE      1
#define CM_GET_CONNECTION_STATUS 2

#define    CM_FDSELECT_SET     FD_SET
#define    CM_FDSELECT_CLR     FD_CLR
#define    CM_FDSELECT_ISSET   FD_ISSET
#define    CM_FDSELECT_ZERO    FD_ZERO

#define  CM_TIMEOUT_SET(a,nSecs,nMSecs)     (a)->time_out.tv_sec  = nSecs;  (a)->time_out.tv_usec = (nMSecs*1000)
#define  CM_TIMEOUT_GET_SECS(a)             (a)->time_out.tv_sec
#define  CM_TIMEOUT_SET_SECS(a,nSecs)       (a)->time_out.tv_sec = nSecs
#define  CM_TIMEOUT_GET_MSECS(a)            (a)->time_out.tv_usec/1000)
#define  CM_TIMEOUT_SET_MSECS(a,nSecs)      (a)->time_out.tv_usec = (nSecs*1000)

#define  CM_TIMEOUT_EMPTY(a)                (a)->time_out.tv_sec = 0; (a)->time_out.tv_usec = 0
#define  CM_TIMEOUT_ISEMPTY(a)              (((a)->time_out.tv_sec <= 0)&&((a)->time_out.tv_usec/1000 <= 0))

#define PROC_V4_PORT_FILE "/proc/sys/net/ipv4/ip_local_port_range"

#define ERROR(str,fmt...) printf("[%s](), line %d" ##str "\r\n",\
                                                   __FUNCTION__,\
                  ##fmt);

int32_t cm_set_port_range (uint16_t lower_limit_ui, uint16_t upper_limit_ui);
int32_t cm_get_port_range (uint16_t * lower_limit_p, uint16_t * upper_limit_p);

int32_t cm_fd_select (uint16_t max_dpl_ui, cm_fd_set_t * read_fds_p,
                     cm_fd_set_t * write_fds_p, cm_fd_set_t * except_fds_p,
                     struct cm_time_out * time_out_p);

#else

#define    cm_fd_set_t         fd_set

#define CM_SO_KEEPALIVE           SO_KEEPALIVE
#define CM_SO_OOBINLINE           SO_OOBINLINE
#define CM_SO_DEBUG               SO_DEBUG
#define CM_SO_BROADCAST           SO_BROADCAST
#define CM_SO_REUSEADDR           SO_REUSEADDR
#define CM_SO_SNDBUF              SO_SNDBUF
#define CM_SO_RCVBUF              SO_RCVBUF
#define CM_SO_LINGER              SO_LINGER
#define CM_IP_OPTIONS             IP_OPTIONS
#define CM_IP_TOS                 IP_TOS
#define CM_IP_TTL                 IP_TTL
#define CM_IP_MULTICAST_TTL       IP_MULTICAST_TTL
#define CM_IP_MULTICAST_LOOP      IP_MULTICAST_LOOP
#define CM_IP_ADD_MEMBERSHIP   IP_ADD_MEMBERSHIP
#define CM_IP_DROP_MEMBERSHIP  IP_DROP_MEMBERSHIP
#define CM_IP_MULTICAST_IF         IP_MULTICAST_IF

int32_t cm_fd_select (uint16_t max_dpl_ui, cm_fd_set_t * read_fds_p,
                     cm_fd_set_t * write_fds_p, cm_fd_set_t * except_fds_p,
                     struct cm_time_out * time_out_p);
#endif /* _UCMSOCK_H_ */
/*
 * Socket API prototypes:
 */
cm_sock_handle cm_socket_create (unsigned char protocol_ui);
int32_t cm_socket_bind (cm_sock_handle sock_fd_i, uint32_t local_ip,
                       uint16_t local_port_ui);
int32_t cm_socket_connect (cm_sock_handle sock_fd_i, uint32_t peer_ip,
                          uint16_t peer_port_ui);
int32_t cm_socket_send (cm_sock_handle sock_fd_i, char * buffer_p,
                       int32_t buf_len_i, int16_t flags_i);
int32_t cm_socket_send_to (cm_sock_handle sock_fd_i, char * buffer_p,
                         int32_t buf_len_i, int16_t flags_i, uint32_t peer_ip,
                         uint16_t peer_port_ui);
int32_t cm_socket_recv (cm_sock_handle sock_fd_i, char * buffer_p,
                       int32_t buf_len_i, int16_t flags_i);
int32_t cm_socket_recv_from (cm_sock_handle sock_fd_i, char * buffer_p,
                           int32_t buf_len_i, int16_t flags_i, uint32_t * remote_ip_p,
                           uint16_t * remote_port_ui_p);
int32_t cm_socket_close (cm_sock_handle sock_fd_i);

int32_t cm_socket_shutdown (cm_sock_handle sock_fd_i, int32_t how_i);

int32_t cm_fd_poll (uint16_t num_desc_ui, struct cm_poll_info * fds_to_be_polled_p,
                   uint32_t time_out_ui);

cm_sock_handle cm_socket_accept (cm_sock_handle sock_fd_i, uint32_t * remote_ip,
                               uint16_t * remote_port_ui);
int32_t cm_socket_listen (cm_sock_handle sock_fd_i, int32_t backlog_i);
int32_t cm_socket_make_non_blocking (cm_sock_handle sock_fd_i);
unsigned char cm_socket_is_would_block (cm_sock_handle sock_fd_i);
int32_t cm_socket_get_sock_name (cm_sock_handle fd_i, uint32_t * ip_p,
                              uint16_t * port_p);

int32_t cm_socket_get_peer_name (cm_sock_handle fd_i, uint32_t * ip_p,
                              uint16_t * port_p);

int32_t cm_socket_get_peer_info (int32_t stream_sock_i, char ** remote_host_c,
                              char ** remote_ip_a);

int32_t cm_get_sock_name (int32_t stream_sock_i);

int32_t cm_set_tcp_keep_alive_options (cm_sock_handle stream_sock_i,
                                   struct cm_tcp_keep_alive_options_info * options_info_p);
int32_t cm_get_tcp_keep_alive_options (cm_sock_handle stream_sock_i,
                                   struct cm_tcp_keep_alive_options_info * options_info_p);

int32_t cm_set_sock_linger_option (cm_sock_handle stream_sock_i,
                                struct cm_socket_linger_option_info * options_info_p);

int32_t cm_get_sock_linger_option (cm_sock_handle stream_sock_i,
                                struct cm_socket_linger_option_info * options_info_p);

int32_t cm_socket_set_opt (int32_t sock_fd_i, uint32_t command_ui, void * pArg);
int32_t cm_socket_get_opt (int32_t sock_fd_i, uint32_t command_ui, void * pArg);

void cm_set_sock_reuse_opt (cm_sock_handle sock_fd_i);

/* 
 * Changed the arguments to this function considering
 * requirements from other modules viz., IGMP.
 */
int32_t cm_set_sock_opt_for_mcast (cm_sock_handle sock_fd, char * iface_name_p,
                               unsigned char * multicast_addr_p, unsigned char list_opt_uc,
                               uint16_t num_srcs_ui, void * src_list_p,
                               int32_t option_name);
int32_t cm_set_ttl (cm_sock_handle sock_fd, int32_t ttl_val_i);

int32_t cm_set_sock_ip_hdr_inc_opt (cm_sock_handle sock_fd_i);
int32_t cm_set_sock_mcast_ttl (cm_sock_handle sock_fd_i, uint32_t ttl_ui);
int32_t cm_set_sock_mcast_loop (cm_sock_handle sock_fd_i, unsigned char loop_uc);
int32_t cm_join_mcast_group_on_iface (int32_t sock_fd_i, uint32_t src_addr,
                                  uint32_t dest_addr, uint32_t if_index_ui);
int32_t cm_leave_mcast_group_on_iface (int32_t sock_fd_i, uint32_t src_addr,
                                   uint32_t dest_addr, uint32_t if_index_ui);
int32_t cm_set_sock_mcast_out_iface (int32_t sock_fd_i, uint32_t src_addr,
                                 uint32_t dest_addr, uint32_t if_index_ui);

int32_t cm_get_socket_option (cm_sock_handle iSockid, int32_t level_i, int32_t opt_name_i,
                                                              int32_t *opt_value_p);

int32_t cm_set_socket_option (cm_sock_handle iSockid, int32_t level_i, int32_t opt_name_i,
                                                              int32_t opt_value_i);
#endif /* _UCMSOCK_H_ */
