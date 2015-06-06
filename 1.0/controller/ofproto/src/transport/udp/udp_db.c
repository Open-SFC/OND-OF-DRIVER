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

/* File  :      udp_api.c
 * Author:      Atmaram G <atmaram.g@freescale.com>
 * Date:   08/23/2013
 * Description: UDP plugin APIs.
 */

/******************************************************************************
 * * * * * * * * * * * * Include Files * * * * * * * * * * * * * * * * * *
 *******************************************************************************/


#include "controller.h"
#include "cntlr_epoll.h"
#include "cntrl_queue.h"
#include "of_utilities.h"
#include "of_msgapi.h"
#include "of_multipart.h"
#include "dprm.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_event.h"
#include "dprmldef.h"
#include "cntlr_xtn.h"
#include "timerapi.h"
#include "of_proto.h"
#include "of_udp_proto.h"


/******************************************************************************
 * * * * * * * * * * * * Global Variables * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
uint32_t      udp_hash_buckets_g = UDP_MAX_TABLE_SIZE;
struct   mchash_table* udp_db_hash_table_g = NULL;
void     *udp_db_mempool_g = NULL;


#define UDP_DB_GET_HASH( ip_address, port) \
   udp_hash_db_get_hashval( ip_address, port, udp_hash_buckets_g)

uint32_t hash_count_g=0;
/******************************************************************************
 * * * * * * * * * * * * Function Definitions * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t udp_db_mempool_init();
int32_t udp_db_hash_table();
void udp_free_node_entry_rcu(struct rcu_head *node_p);
int32_t udp_hash_db_get_hashval(ippaddr_t ip_address, uint16_t port, uint32_t no_of_buckets);


int32_t udp_db_init()
{
   int32_t status=OF_SUCCESS;
   int32_t retval;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");

   do
   {
      /** Create mempools for MFC database */
      retval=udp_db_mempool_init();
      if( CNTLR_UNLIKELY(retval != OF_SUCCESS) )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "udp_db_mempool_init  failed");
         status= OF_FAILURE;
         break;
      }

      /** Create hash table to contain MFC rules */
      retval=udp_db_hash_table();
      if( CNTLR_UNLIKELY(retval != OF_SUCCESS) )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "udp_db_hash_table failed");
         status= OF_FAILURE;
         break;
      }

   }while(0);

   return status;
}

void udp_db_get_map_entry_mempoolentries(uint32_t* db_entries_max,uint32_t* db_static_entries)
{
   *db_entries_max    = UDP_MAX_NODE_ENTRIES;
   *db_static_entries = UDP_MAX_NODE_STATIC_ENTRIES;
}



int32_t udp_db_mempool_init()
{
   int32_t retval;
   int32_t status = OF_SUCCESS;
   uint32_t db_entries_max, db_static_entries;
   struct mempool_params mempool_info={};

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "entered");
   do
   {
      udp_db_get_map_entry_mempoolentries(&db_entries_max,&db_static_entries);
      
      mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
      mempool_info.block_size = sizeof(struct udp_db_entry);
      mempool_info.max_blocks = db_entries_max;
      mempool_info.static_blocks = db_static_entries;
      mempool_info.threshold_min = db_static_entries/10;
      mempool_info.threshold_max = db_static_entries/3;
      mempool_info.replenish_count = db_static_entries/10;
      mempool_info.b_memset_not_required =  FALSE;
      mempool_info.b_list_per_thread_required =  FALSE;
      mempool_info.noof_blocks_to_move_to_thread_list = 0;

      retval = mempool_create_pool("udp_db_pool", &mempool_info,
		      &udp_db_mempool_g
		      );
      if( CNTLR_UNLIKELY(retval != MEMPOOL_SUCCESS) )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "mempool creation failed");
         status = OF_FAILURE;
      }
   }while(0);

   return status;
}

int32_t udp_db_hash_table()
{
   int32_t retval;
   int32_t status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "entered");
   do
   {
      /** Create hash table to save udp records */
      retval = mchash_table_create(udp_hash_buckets_g,
            UDP_MAX_NODE_ENTRIES,
            udp_free_node_entry_rcu,
            &udp_db_hash_table_g);
      if(retval != MCHASHTBL_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "hash table creation failed");
         status = OF_FAILURE;
         break;
      }
   }while(0);

   return status;
}

void udp_free_node_entry_rcu(struct rcu_head *node_p)
{
   uchar8_t offset;
   struct udp_db_entry *db_entry_p;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "entry");

   if(node_p)
   {
      offset = UDP_HASH_DB_UDP_ENTRY_HASH_TBLLNK_OFFSET;
      db_entry_p = (struct udp_db_entry *) ( (char *)node_p -(RCUNODE_OFFSET_IN_MCHASH_TBLLNK) - offset);
      mempool_release_mem_block(udp_db_mempool_g, (uchar8_t*)db_entry_p, db_entry_p->heap_b);
      return;
   }
   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN, "NULL Passed for deletion");
}

int32_t udp_hash_db_get_hashval(ippaddr_t ip_address, uint16_t port, uint32_t no_of_buckets)
{
   uint32_t hash,param1,param2,param3,param4;

   param1 = 1;
   param2 = ip_address.v4_val;
   param3 = port;
   param4 = 1;
   DPRM_COMPUTE_HASH(param1,param2,param3,param4,no_of_buckets,hash);
   return hash;
}


int32_t udp_hash_db_get_db_entry(ippaddr_t ip_address, uint16_t port,struct    udp_db_entry **db_entry_pp)
{
   uint8_t   offset;
   uint32_t  hash;
   struct    udp_db_entry *db_entry_p;
   int32_t status = OF_FAILURE;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "entered");
   do
   {
      offset = UDP_HASH_DB_UDP_ENTRY_HASH_TBLLNK_OFFSET;
      hash=UDP_DB_GET_HASH(ip_address, port);
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_INFO, "ipaddress 0x%x  port %d hashcount %d",  ip_address.v4_val, port, hash_count_g);
      MCHASH_BUCKET_SCAN(udp_db_hash_table_g, hash, db_entry_p, struct udp_db_entry*, offset)
      {
         if(!((ip_address.v4_val == db_entry_p->ip_address.v4_val) && (port == db_entry_p->port)))
         {
            continue;
         }
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "Found a match");
         status=OF_SUCCESS;
         *db_entry_pp=db_entry_p;
         break;
      }
   }while (0);

   return status;
}


int32_t udp_hash_db_add_db_entry(ippaddr_t ip_address, uint16_t port, struct udp_db_entry **db_entry_pp)
{
   uint32_t  hash;
   struct    udp_db_entry *db_entry_p;
   int32_t   retval;
   int32_t status = OF_SUCCESS;
   uint8_t heap_b;
   int32_t magic,index;
   uchar8_t        *hashobj_p = NULL;
   int8_t status_b;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "entered");
   do
   {

      retval = mempool_get_mem_block(udp_db_mempool_g, (uchar8_t**)&db_entry_p, &heap_b);
      if(retval != MEMPOOL_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "mempool get block failed");
         retval = OF_FAILURE;
         break;
      }
      db_entry_p->heap_b = heap_b;

      hash=UDP_DB_GET_HASH(ip_address, port);

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_INFO, "ipaddress 0x%x  port %d",  ip_address.v4_val, port);
      db_entry_p->ip_address = ip_address;
      db_entry_p->port = port;
      db_entry_p->read_len=0;
      db_entry_p->msg_len=0;
      memset(db_entry_p->read_buffer, 0, UDP_MAX_BUFFER_LENGTH);

      hashobj_p = (uchar8_t *)db_entry_p + UDP_HASH_DB_UDP_ENTRY_HASH_TBLLNK_OFFSET;
      MCHASH_APPEND_NODE(udp_db_hash_table_g, hash, db_entry_p, index, magic, hashobj_p, status_b);
      if ( CNTLR_LIKELY(status_b == TRUE ))
      {
         db_entry_p->magic= magic;
         db_entry_p->index= index;
         *db_entry_pp=db_entry_p;
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "succesfully appended node hashcount %d",  ++hash_count_g);
      }
      else
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "append node failed");
         status=OF_FAILURE;
         break;
      }

   }while (0);

   return status;
}



int32_t udp_hash_db_del_db_entry( ippaddr_t ip_address, uint16_t port)
{
   uint8_t   offset;
   uint32_t  hash;
   struct    udp_db_entry *db_entry_p=NULL;
   int32_t status = OF_FAILURE;
   int8_t status_b;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "entered");
   do
   {
      offset = UDP_HASH_DB_UDP_ENTRY_HASH_TBLLNK_OFFSET;
      hash=UDP_DB_GET_HASH(ip_address, port);

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_INFO, "ipaddress 0x%x  port %d",  ip_address.v4_val, port);
      MCHASH_BUCKET_SCAN(udp_db_hash_table_g, hash, db_entry_p, struct udp_db_entry *, offset)
      {
         if ((db_entry_p->ip_address.v4_val == ip_address.v4_val)
               && (db_entry_p->port == port))
         {
      	   // memset(db_entry_p->read_buffer, 0, UDP_MAX_BUFFER_LENGTH);
            MCHASH_DELETE_NODE(udp_db_hash_table_g, db_entry_p, offset, status_b);
            if(status_b == FALSE)
            {
               status=OF_FAILURE;
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "Failed to delete node");
            }
            else
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "Succesfully  deleted node nodecount %d",--hash_count_g);
               status=OF_SUCCESS;
            }
            break;
         }
      }

   }while (0);

   return status;

}

