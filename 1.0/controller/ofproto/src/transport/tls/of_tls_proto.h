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

/* File  :      of_tls_proto.c
 * Author:     Vivek Sen Reddy K 
 * Date:   Jan 2014
 * Description: TLS protocol specific structures and funtion protos.
 */


/******************************************************************************
*********************  Structure Definitions *************************************
******************************************************************************/
/** TCP protocol connection details  */
typedef struct tcp_conn_details_s
{
   ippaddr_t            remote_ip;
   uint16_t             remote_port;
}tcp_conn_details_t;

/*TCP protocol Details */
typedef struct tcp_proto_details_s
{
   uint16_t port_number;
}tcp_proto_details_t;



/** TLS connection details structure */
typedef struct tls_conn_details_s
{   
   ippaddr_t               remote_ip;
   uint16_t                remote_port;
   cntlr_ssl_info_t     pssl_info;
}tls_conn_details_t;                    

/*TLS protocol Details */
typedef struct tls_proto_details_s
{
   uint16_t port_number;
}tls_proto_details_t;



/******************************************************************************
 * * * * * * * * * * * * Function Prototypes * * * * * * * * * * * * * * * * * *
 *******************************************************************************/

int32_t of_transport_proto_init(void);
int32_t cntlr_register_tls_proto(void);
int32_t cntlr_unregister_tls_proto(void);


/******************* TCP Functions ******************************************/
int32_t tcp_transport_init (of_transport_proto_properties_t *transport_proto);
int32_t tcp_transport_open (int32_t fd, of_conn_properties_t *conn_details_p);
int32_t tcp_transport_write_data (int32_t fd,
        void *conn_spec_info,
      void        *msg,
      uint16_t                length,
      int32_t flags,
      uint16_t               *len_write,
        uint8_t              *peer_closed_conn);
int32_t tcp_transport_read_data (int32_t fd,
        void *conn_spec_info,
      void                *msg,
      uint16_t               length,
      int32_t flags,
      uint16_t               *len_read,
        uint8_t              *peer_closed_conn,
	void **opaque);

int32_t tcp_transport_get_hash_value (void *conn_spec_info, int32_t *hash_p);

int32_t tcp_transport_compare_conn_spec_info (void *conn_spec_info_one,void *conn_spec_info_two, int8_t *hit_p);

int32_t tcp_transport_compare_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void *proto_spec , int8_t *hit_p);

int32_t tcp_transport_alloc_and_set_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void **proto_spec);

int32_t tcp_transport_get_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void *proto_spec);

int32_t tcp_transport_close (int32_t fd,void *conn_spec_info);
int32_t tcp_transport_deinit (int32_t fd);

/******************* TLS Functions ******************************************/
int32_t tls_transport_init (of_transport_proto_properties_t *transport_proto);
int32_t tls_transport_open (int32_t fd, of_conn_properties_t *conn_details_p);
int32_t tls_transport_write_data (int32_t fd,
      void *conn_spec_info,
      void        *msg,
      uint16_t                length,
      int32_t flags,
      uint16_t               *len_write,
      uint8_t              *peer_closed_conn);
int32_t tls_transport_read_data (int32_t fd,
      void *conn_spec_info,
      void                *msg,
      uint16_t               length,
      int32_t flags,
      uint16_t               *len_read,
      uint8_t              *peer_closed_conn,
      void **opaque);

extern int32_t tls_transport_get_hash_value (void *conn_spec_info, int32_t *hash_p);

extern int32_t tls_transport_compare_conn_spec_info (void *conn_spec_info_one,void *conn_spec_info_two, int8_t *hit_p);

int32_t tls_transport_compare_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void *proto_spec , int8_t *hit_p);

int32_t tls_transport_alloc_and_set_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void **proto_spec);

int32_t tls_transport_get_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void *proto_spec);

int32_t tls_transport_close (int32_t fd,void *conn_spec_info);
int32_t tls_transport_deinit (int32_t fd);

