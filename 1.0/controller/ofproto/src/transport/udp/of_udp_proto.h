
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

/* File  : of_tcp_proto.h
 * Author: Vivek Sen Reddy .K
 * Date  : Jan , 2014  
 * Description: 
 *  This file contains ucp protocol specific definitions and structures.
 * History:
 */


/******************************************************************************
*********************  Structure Definitions *************************************
******************************************************************************/

#define UDP_MAX_BUFFER_LENGTH   (8 * 1024)
#define UDP_MAX_NODE_ENTRIES 1024
#define UDP_MAX_NODE_STATIC_ENTRIES 256
#define UDP_MAX_TABLE_SIZE  (UDP_MAX_NODE_ENTRIES/8)+1

struct udp_db_entry
{
        struct mchash_table_link hash_tbl_link;
        ippaddr_t               ip_address;
        uint16_t                port;
        uint32_t index;   /* index of the safe reference to this node entry in mchash table */
        uint32_t magic;   /* index of the safe reference to this node entry in mchash table */
        char read_buffer[UDP_MAX_BUFFER_LENGTH];
        uint32_t msg_len;
        uint32_t read_len;
	int8_t heap_b;
	int8_t processed_b;
};

#define UDP_HASH_DB_UDP_ENTRY_HASH_TBLLNK_OFFSET offsetof(struct udp_db_entry, hash_tbl_link);


/**UDP conn details structure*/
typedef struct udp_conn_details_s
{  
  ippaddr_t               remote_ip;
  uint16_t                remote_port; 
} udp_conn_details_t;                    

/*UDP protocol Details */
typedef struct udp_proto_details_s
{
   uint16_t port_number;
}udp_proto_details_t;


/******************************************************************************
 * * * * * * * * * * * * Function Prototypes * * * * * * * * * * * * * * * * * *
 *******************************************************************************/

int32_t of_transport_proto_init(void);
int32_t cntlr_register_udp_proto(void);
int32_t cntlr_unregister_udp_proto(void);


int32_t udp_transport_init (of_transport_proto_properties_t *transport_proto);
int32_t udp_transport_open (int32_t fd, of_conn_properties_t *conn_details_p);
int32_t udp_transport_write_data (int32_t fd,
     void *conn_spec_info_p,
      void        *msg,
      uint16_t                length,
      int32_t flags,
      uint16_t               *len_write,
      uint8_t              *peer_closed_conn);
int32_t udp_transport_read_data (int32_t fd,
     void *conn_spec_info_p,
      void                *msg,
      uint16_t               length,
      int32_t flags,
      uint16_t               *len_read,
      uint8_t              *peer_closed_conn,
      void **opaque);

int32_t udp_transport_get_hash_value (void *conn_spec_info, int32_t *hash_p);

int32_t udp_transport_compare_conn_spec_info (void *conn_spec_info_one,void *conn_spec_info_two, int8_t *hit_p);


int32_t udp_transport_compare_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void *proto_spec , int8_t *hit_p);

int32_t udp_transport_alloc_and_set_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void **proto_spec);

int32_t udp_transport_get_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void *proto_spec);

int32_t udp_transport_close (int32_t fd, void *conn_spec_info_p);

int32_t udp_transport_deinit (int32_t fd);



/******************************************************************************
 * * * * * * * * * * * * Function Prototypes * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t udp_db_init();

int32_t udp_hash_db_get_db_entry(ippaddr_t ip_address, uint16_t port,struct    udp_db_entry **db_entry_pp);
int32_t udp_hash_db_add_db_entry(ippaddr_t ip_address, uint16_t port, struct udp_db_entry **db_entry_pp);
int32_t udp_hash_db_del_db_entry( ippaddr_t ip_address, uint16_t port);

