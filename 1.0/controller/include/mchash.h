/* 
  *
  * Copyright  2012, 2013  Freescale Semiconductor
  *
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
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
 * File name:  mchash.h
 * Author: Freescale Semiconductor    
 * Date:   01/20/2013
 * Description: This file contains API functions implementing multi core hash table functionality. 
 * API allows Applications to create,delete multiple hash tables each holding a given number of Application nodes 
 * Support is provided to place an Application node in multiple hash tables.
 * Table scan,bucket scan and append node to a hash table macros are supported.
 * Delete a node from hash table and delete a node from hash table using its safe reference are also supported. 
 */

//! Multicore Hashtable database API
/*! This module provides DP resource APIs
 * \addtogroup Hashtable_Management
 */

#ifndef __MC_HASH_H
#define __MC_HASH_H

#define X86_CACHE_LINE_SIZE 32

/** \ingroup Hashtable_Management
  * MCHASH_MAX_HASHBUCKET_CACHE_NODES
  *  - Returns maximum number of Application nodes that can be cached per bucket.
  */
#define MCHASH_MAX_HASHBUCKET_CACHE_NODES  10

/** \ingroup Error_Management
  * MCHASHTBL_SUCCESS
  * - MCHASHTBL API return this value on successful execution
  */
#define MCHASHTBL_SUCCESS 0

/** \ingroup Error_Management
  * MCHASHTBL_FAILURE
  * - MCHASHTBL API return this value on failure in execution
  */
#define MCHASHTBL_FAILURE -50

/** \ingroup Error_Management
  * MCHASHTBL_ERROR_NO_MEM 
  * - API Functions return this value when memory allocation fails.
  */
#define MCHASHTBL_ERROR_NO_MEM -51

/** \ingroup Error_Management
 * MCHASHTBL_ERROR_NOT_EMPTY
 * - API mchash_table_delete() returns this value if hash table is not empty for deletion
 */
#define MCHASHTBL_ERROR_NOT_EMPTY -52

/** \ingroup  Hashtable_Management
  \typedef mchash_free_func
  \brief Prototype for the Nodefree callback function to be used by Applications.\n
  <b>Description</b>\n 
  This callback function is called by the hash table module before deleting it from its internal lists.
  When called, the Application shall free the Application node.
 */
typedef void (*mchash_free_func)(struct rcu_head* objnode_p);

/** \ingroup Hashtable_Management
  \struct mchash_dll_node 
  \brief Node used to link Application nodes using a double linked list.  
 */
struct mchash_dll_node{
  /** links to the previous node in the list */
  struct mchash_dll_node *prev_p;
  /** links to the next node in the list */
  struct mchash_dll_node *next_p;
};

/** \ingroup Hashtable_Management 
  \struct mchash_table_link 
  \brief It is used by mchash table infrastructure to attach a given application node to a specific hash table.\n
  <b>Description</b>\n
  This structure is used by the mchash table infrastructure.
  This structure is used to attach an application node to a given hash table.
  The application node data structure shall instantiate this link structure as many times
  as the number of hash tables to which the node is to be attached.
  The offset in bytes of this link node from the beginning of the Application node is to be
  passed as an argument in many of the mchash API.\n
 */
struct mchash_table_link{
  /** points to the previous node in the hash table */ 
  struct   mchash_dll_node  *prev_p;
  /** points to the next node in the hash table */
  struct   mchash_dll_node  *next_p;
  /** rcu anchor in the node */
  struct   rcu_head rcu_head;
  /** index of the internal array node holding the Application node */
  uint32_t index;
  /** Hash value identifying the bucket index for deletion purpose*/
  uint32_t key;
};

/** \ingroup Hashtable_Management
  \struct mchash_noderef 
  \brief  Allows faster and safe reference of the Application node.\n
  <b>Description</b>\n
  The values of index amd magic number are returned to Application for faster and 
  safer reference of its node at a later time. \n
  */
struct mchash_noderef{
  /** index of the internal array node holding the Application node and magic number */
  uint32_t  index;
  /** magic number is incremented each time the contents of Application node are modified */
  uint32_t  magic;
};

/** \ingroup Hashtable_Management  
  \struct array_indirect_entry 
  \brief Contains the actual Application node and the magic number.\n
  <b>Description</b>\n
  These nodes are placed in an array and the index is placed in the link node
  attached to the hash table. \n  
 */
struct array_indirect_entry{
  /** points to the previous node in the list */
  struct  mchash_dll_node  *prev_p;
  /** points to the next node in the list */
  struct  mchash_dll_node  *next_p;
  /** magic number of the Application node */
  uint32_t  magic;
  /** pointer to the Application node */
  void  *node_p;
};

/** \ingroup Hashtable_Management  
  \struct mchash_bucket
  \brief Bucket in the hash table \n
  <b>Description</b>\n
  Application nodes with the same hash value are placed in the same bucket.\n
 */ 
struct mchash_bucket{
  /** Head node of linked list in the bucket */
  struct mchash_dll_node head;
  /** Bucket level lock */
  cntlr_lock_t mchash_bucket_lock;
  /** Number of Application nodes in the bucket */
  uint32_t    count;
  /** Nodes are prefetched for faster access */
  ulong    prefetch_nodes[MCHASH_MAX_HASHBUCKET_CACHE_NODES];
};

/** \ingroup Hashtable_Management 
  \struct mchash_table
  \brief Hash table structure 
   <b>Description</b>
   Hash table structure that makes use of array based indirection and RCU locks. \n

  */
struct mchash_table{
  /** Input argument: Number of buckets in the hash table */
  uint32_t                     buckets;
  /** Input argument: max no of Application nodes in the hash table */ 
  uint32_t                     max_nodes;
  /** For internal purpose */
  uint32_t                     active_nodes;
  /** For internal purpose */
  uint32_t                     passive_nodes;
  /** For internal purpose */
  uint32_t                     magic_no;
  /** Table level lock */
  cntlr_lock_t                 mchash_table_lock;
  /** Queue holding free nodes in array indirection table */ 
  struct mchash_dll_node       free_q;
  /** Points to array indirection table entry. */
  struct array_indirect_entry* indirection_array_p;
  /** Registered function: Called when a node is deleted by Application */
  mchash_free_func             free_func_cbk;
  /** Internal purpose */
  void                         *table_base_p;
  /** Array of bucket structures */
  struct mchash_bucket         bucket_list[1];
};

/** \ingroup Hashtable_Management
  \brief prefetch application nodes into cache memory.\n

  \param[in] hashbucket_p - Bucket structure 
 */
static inline void mchash_update_prefetch_nodes(struct mchash_bucket* hashbucket_p)
{
  uint32_t ii;
  struct mchash_dll_node* node_p;

  node_p = hashbucket_p->head.next_p;
  for(ii = 0; ii < MCHASH_MAX_HASHBUCKET_CACHE_NODES; ii++)
  {
    if(node_p !=  NULL)
    {
      hashbucket_p->prefetch_nodes[ii] = (ulong)node_p;
      node_p = node_p->next_p;
    }
    else
     break;
  }
  for(; ii < MCHASH_MAX_HASHBUCKET_CACHE_NODES; ii++)
    hashbucket_p->prefetch_nodes[ii] = 0;
}

/** \ingroup Hashtable_Management
  \brief This function creates a hash table.\n
  <b>Description:</b>\n
  This function creates a hash table by using the input parameters of
  number of buckets required in the hash table and the number of nodes required.
  It returns a pointer to the hash table created and shall be passed as an argument
  for all the other mchash module API.
  The Application shall pass a double pointer to the hash table structure so that
  hash table pointer is copied.

  \param[in]      no_of_buckets     - Number of buckets to be created for the hash table. 
  \param[in]      max_nodes         - Maximum number of Application Nodes that can be added to the hash table. 
  \param[in]      freefunc_cbk      - This registered callback function is called when the Application deletes a node in the hash table.
  \param[in,out]  hash_table_p_p    - A pointer to the hash table created is copied back using this input double pointer to hash table.  

  \return MCHASHTBL_SUCCESS         - Returns this value upon successful creation of the hash table.
  \return MCHASHTBL_ERROR_NO_MEM    - Returns this value when memory allocation failure occurs. 
 */
int32_t  mchash_table_create(uint32_t               no_of_buckets,
                             uint32_t               max_nodes,
                             mchash_free_func       freefunc_cbk,
                             struct mchash_table**  hash_table_p_p);

/** \ingroup Hashtable_Management
  \brief This function deletes a hash table.\n
  <b>Description:</b>\n
  This API deletes a hash table.This API expects the hash table pointer that it returned earlier during hash table creation.
  The Application is assumed to free all the hash table nodes before calling this API.
  
  \param[in]  hash_table_p          - Pointer to the hash table to be deleted.

  \return MCHASHTABLE_SUCCESS       - Upon successful deletion of the hash table.
  \return MCHASHTABLE_FAILURE       - Upon unsuccessful deletion of the hash table.
  \return MCHASHTBL_ERROR_NOT_EMPTY - Returns this value when hash table is not empty for deletion.
 */
int32_t  mchash_table_delete(struct mchash_table* hash_table_p);

void mchash_insert_node_in_middle(struct mchash_bucket* bucket_p,struct mchash_dll_node *prev_p,
                                  struct mchash_dll_node *node_p, struct mchash_dll_node* next_p);
/* MACROS */
/** \ingroup Hashtable_Management
  <b>Description:</b>\n
  This macro appends the Application node to the hash table bucket determined by the input parameter "hash_key".
  A safe reference to the Application node for faster access (mchash_noderef_index,mchash_noderef_magic) is returned.
  \param[in]   hash_table_p         - Pointer to the hash table which is returned during its creation.
  \param[in]   keys                 - Hash value calculated by Application to select the bucket for appending.
  \param[in]   nodeobj_p            - Pointer to the Application Node to be appended.
  \param[in]   hashobj_p            - pointer to the specific mchash_table_link structure in the node for appending 
                                    - the node to one of the many hash tables to which the node is attached.  
  \param[out]  mchash_noderef_index - Index field of the safe reference to the Application node.  
  \param[out]  mchash_noderef_magic - Magic field of the safe reference to the Application node.
  \param[out]  status               - TRUE in case of successful deletion, FALSE in case of failure.  
 */
#if 1 
#define MCHASH_BUCKET_NODE_COUNT(hash_table_p,keys, count) \
{\
	struct mchash_bucket* bucket_p;\
	cntlr_assert(hash_table_p);\
	CNTLR_LOCK_TAKE(hash_table_p->mchash_table_lock); \
	bucket_p = &((hash_table_p)->bucket_list[(keys)]); \
	CNTLR_LOCK_RELEASE(hash_table_p->mchash_table_lock); \
	CNTLR_LOCK_TAKE(bucket_p->mchash_bucket_lock); \
	count= bucket_p->count;\
	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"number of nodes %d in bucket %d",bucket_p->count,keys);\
	CNTLR_LOCK_RELEASE(bucket_p->mchash_bucket_lock); \
}
#else 
#define MCHASH_BUCKET_NODE_COUNT(hash_table_p,keys, count) 
#endif 

#define MCHASH_APPEND_NODE(hash_table_p,keys,nodeobj_p,mchash_noderef_index,mchash_noderef_magic,hashobj_p,status)\
{\
  struct mchash_bucket* bucket_p;\
  struct array_indirect_entry* array_node_p;\
  cntlr_assert(hash_table_p);\
  cntlr_assert(nodeobj_p); \
  status = FALSE; \
  CNTLR_LOCK_TAKE(hash_table_p->mchash_table_lock); \
  if(hash_table_p->free_q.next_p != NULL) \
  { \
  /*Take free Indirection Node, Assign Magic Number & node_p pointer,\
    Increament uiActiveNodes & magic*/ \
    array_node_p = (struct array_indirect_entry *)hash_table_p->free_q.next_p;\
    hash_table_p->free_q.next_p = array_node_p->next_p;\
    if(hash_table_p->free_q.next_p == NULL) \
      hash_table_p->free_q.prev_p = NULL; \
    else \
      (hash_table_p->free_q.next_p)->prev_p = NULL; \
    array_node_p->magic = hash_table_p->magic_no; \
    hash_table_p->magic_no++; \
    if(0 == hash_table_p->magic_no) hash_table_p->magic_no = 1; \
    hash_table_p->active_nodes++; \
    CNTLR_LOCK_RELEASE(hash_table_p->mchash_table_lock); \
    array_node_p->node_p = nodeobj_p; \
    bucket_p = &((hash_table_p)->bucket_list[(keys)]); \
    mchash_noderef_magic = array_node_p->magic; \
    mchash_noderef_index = array_node_p - hash_table_p->indirection_array_p;\
    ((struct mchash_table_link *)hashobj_p)->index = mchash_noderef_index; \
    CNTLR_LOCK_TAKE(bucket_p->mchash_bucket_lock); \
    mchash_insert_node_in_middle(bucket_p,bucket_p->head.prev_p,(struct mchash_dll_node *)hashobj_p,NULL);\
    ((struct mchash_table_link*)hashobj_p)->key = keys;\
    bucket_p->count++;                                                        \
    CNTLR_LOCK_RELEASE(bucket_p->mchash_bucket_lock); \
    status = TRUE; \
  } \
  else \
    CNTLR_LOCK_RELEASE(hash_table_p->mchash_table_lock); \
}  

/** \ingroup Hashtable_Management
  <b>Description:</b>\n
  This macro returns the Application node using the inputs of "mchash_noderef_index"
  and the "mchash_noderef_magic" associated with the node.  
  \param[in]  hash_table_p          - Pointer to the hash table which is returned during its creation.
  \param[in]  noderef_index         - Index field of the safe reference to the Application node.  
  \param[in]  noderef_magic         - Magic field of the safe reference to the Application node.
  \param[out] nodeobj_p             - Pointer to the Application Node.
  \param[out] status                - TRUE in case of successful deletion, FALSE in case of failure.
 */
#define MCHASH_ACCESS_NODE(hash_table_p,noderef_index,noderef_magic,nodeobj_p,status)\
{\
  struct array_indirect_entry* noderef_p = (&(hash_table_p->indirection_array_p[noderef_index]));\
  status = FALSE;\
  if(noderef_magic == noderef_p->magic)\
   {\
      nodeobj_p = rcu_dereference(noderef_p->node_p);\
      status = TRUE;\
   }\
}
/** \ingroup Hashtable_Management
  <b>Description:</b>\n
  This macro deletes the Application node using the safe reference to the node.
  It first deletes the node from its internal lists before submitting it to call_rcu.
  The Application node shall free the node in call_rcu callback function.
  \param[in] hash_table_p          - Pointer to the hash table which is returned during its creation.
  \param[in] mchash_noderef_index  - Index field of the safe reference to the Application node.  
  \param[in] mchash_noderef_magic  - Magic field of the safe reference to the Application node.
  \param[in] typecast              - Application Node structure typecast name.
  \param[in] offset                - Number of bytes from beginning of the node to mchash_table_link structure in the node 
                                   - using which the node is attached to a given hash table.  
  \param[out] status               - returns TRUE in case of successful deletion, FALSE in case of failure.
  */
#define MCHASH_DELETE_NODE_BY_REF(hash_table_p,mchash_noderef_index,mchash_noderef_magic,typecast,offset,status)\
{                                                                                \
  struct mchash_bucket  *bucket_p ;                                                   \
  struct array_indirect_entry *array_node_p;                                           \
  struct mchash_dll_node *prev_p=NULL, *next_p=NULL;                                   \
  struct mchash_table_link *table_link;\
  cntlr_assert(hash_table_p);                                                          \
  status = FALSE;                                                                \
  array_node_p=(&(hash_table_p->indirection_array_p[mchash_noderef_index]));              \
  if(mchash_noderef_magic == array_node_p->magic)                                 \
  {                                                                              \
    table_link = (struct mchash_table_link *)((unsigned char*)(array_node_p->node_p) + offset);\
    bucket_p = &(hash_table_p)->bucket_list[table_link->key];\
    CNTLR_LOCK_TAKE(bucket_p->mchash_bucket_lock);                          \
    if(array_node_p->magic != 0)                                                 \
    {                                                                            \
      array_node_p->magic = 0;                                                   \
      next_p = table_link->next_p;\
      prev_p = table_link->prev_p;\
      if (prev_p == NULL)                                                         \
       prev_p = &bucket_p->head;                        \
      CNTLR_RCU_ASSIGN_POINTER(prev_p->next_p, (struct mchash_dll_node *)next_p);        \
      if (next_p == NULL)                                                         \
      {                                                                          \
        next_p =  &bucket_p->head;                      \
      }                                                                          \
      CNTLR_RCU_ASSIGN_POINTER(next_p->prev_p, (struct mchash_dll_node *)prev_p);        \
      CNTLR_CALL_RCU((struct rcu_head *)&table_link->rcu_head,hash_table_p->free_func_cbk);    \
      array_node_p->node_p = NULL;                                                  \
      bucket_p->count--;                                                        \
      mchash_update_prefetch_nodes(bucket_p);                                     \
      array_node_p->next_p  = NULL;                                                 \
      CNTLR_LOCK_RELEASE(bucket_p->mchash_bucket_lock);                     \
      CNTLR_LOCK_TAKE(hash_table_p->mchash_table_lock);                          \
      hash_table_p->active_nodes--;\
      if(hash_table_p->free_q.prev_p != NULL)                                          \
      {                                                                          \
       array_node_p->prev_p  = hash_table_p->free_q.prev_p;                               \
       (hash_table_p->free_q.prev_p)->next_p = (struct mchash_dll_node *)array_node_p;        \
       hash_table_p->free_q.prev_p = (struct mchash_dll_node *)array_node_p;                 \
      }                                                                          \
      else                                                                       \
      {                                                                          \
        array_node_p->prev_p  = NULL;                                               \
        hash_table_p->free_q.prev_p = hash_table_p->free_q.next_p  = (struct mchash_dll_node *)array_node_p;  \
      }                                                                          \
      CNTLR_LOCK_RELEASE(hash_table_p->mchash_table_lock);                     \
      status = TRUE;                                                             \
    }                                                                            \
    else                                                                         \
      CNTLR_LOCK_RELEASE(bucket_p->mchash_bucket_lock);                     \
  }                                                                              \
}

#define MCHASH_DELETE_NODE(hash_table_p,nodeobj_p,offset,status)                   \
{                                                                                \
  struct mchash_table_link *table_link;\
  struct mchash_bucket  *bucket_p;                                                   \
  struct array_indirect_entry *array_node_p;                                           \
  struct mchash_dll_node *prev_p=NULL, *next_p=NULL;                                   \
  cntlr_assert(nodeobj_p);                                                          \
  status = FALSE;                                                                \
  table_link = (struct mchash_table_link *)((unsigned char*)nodeobj_p + offset);\
  bucket_p = &(hash_table_p)->bucket_list[table_link->key];\
  array_node_p=&(hash_table_p->indirection_array_p[table_link->index]);              \
  CNTLR_LOCK_TAKE(bucket_p->mchash_bucket_lock);                            \
  if(nodeobj_p == array_node_p->node_p)                                              \
  {                                                                              \
      array_node_p->magic  = 0;                                                   \
      array_node_p->node_p = NULL;                                                  \
      next_p = table_link->next_p;\
      prev_p = table_link->prev_p;\
      if (prev_p == NULL)                                                         \
      {                                                                          \
       prev_p = &(bucket_p)->head;                                                 \
      }                                                                          \
      CNTLR_RCU_ASSIGN_POINTER(prev_p->next_p,  (struct mchash_dll_node *)next_p);       \
      if (next_p == NULL)                                                         \
      {                                                                          \
       next_p = &(bucket_p)->head;                                                 \
      }                                                                          \
      CNTLR_RCU_ASSIGN_POINTER(next_p->prev_p,  (struct mchash_dll_node *)prev_p);       \
      bucket_p->count--;                                                        \
      mchash_update_prefetch_nodes(bucket_p);                                     \
      CNTLR_LOCK_RELEASE(bucket_p->mchash_bucket_lock);                     \
      array_node_p->next_p  = NULL;                                                 \
      CNTLR_LOCK_TAKE(hash_table_p->mchash_table_lock);                            \
      hash_table_p->active_nodes--;                                                 \
      /*Insert back to free_q*/                                                   \
      if(hash_table_p->free_q.prev_p != NULL)                                          \
      {                                                                          \
       array_node_p->prev_p  = hash_table_p->free_q.prev_p;                               \
       (hash_table_p->free_q.prev_p)->next_p = (struct mchash_dll_node *)array_node_p;        \
       hash_table_p->free_q.prev_p = (struct mchash_dll_node *)array_node_p;                 \
      }                                                                          \
      else                                                                       \
      {                                                                          \
        array_node_p->prev_p  = NULL;                                               \
        hash_table_p->free_q.prev_p = hash_table_p->free_q.next_p  = (struct mchash_dll_node *)array_node_p;  \
      }                                                                          \
      CNTLR_LOCK_RELEASE(hash_table_p->mchash_table_lock);                     \
      CNTLR_CALL_RCU((struct rcu_head *)&table_link->rcu_head,hash_table_p->free_func_cbk);    \
      status = TRUE;                                                             \
  }                                                                              \
  else                                                                           \
    CNTLR_LOCK_RELEASE(bucket_p->mchash_bucket_lock);                       \
}



/** \ingroup Hashtable_Management
  <b>Description:</b>\n
  This macro is used to parse the hash buckets from the first bucket to the last.
  This macrp provides a 'for' loop providing the next bucket for applications usage.

  \param[in]  hash_table_p   - Pointer to the hash table which is returned during its creation.
  \param[in]  key            - Hash value calculated by Application to select the bucket
 */ 
#define MCHASH_TABLE_SCAN(hash_table_p,key) \
  for ((key = 0); (key) < (hash_table_p)->buckets; (key) = (key) + 1)

/** \ingroup Hashtable_Management
 <b>Description:</b>\n
 This macro is used to parse the given hash bucket from first node to last node.
 This macro provides a 'for' loop providing the next node in the bucket for applications usage.

 \param[in]  hash_table_p         - Pointer to the hash table which is returned during its creation.
 \param[in]  key                  - Hash value calculated by Application to select the bucket.
 \param[in]  nodeobj_p            - Pointer to the Application Node for Application usage.
 \param[in]  typecast             - Application Node structure typecast name.
 \param[in]  offset               - Number of bytes from beginning of the node to mchash_table_link structure in the node 
                                  - used to find the hash table to be scanned.The node may be present in multiple tables.  
 */
#define MCHASH_BUCKET_SCAN(hash_table_p,key,nodeobj_p,typecast,offset)\
 struct mchash_table_link* hash_link_p;\
 for(hash_link_p = (struct mchash_table_link*)((&((hash_table_p)->bucket_list[key]))->head.next_p); \
    ((hash_link_p != NULL) &&(nodeobj_p = rcu_dereference((typecast)((unsigned char*)hash_link_p-offset)))); \
    hash_link_p = (struct mchash_table_link*)hash_link_p->next_p)\


#define MCHASH_BUCKET_GET_HEAD_NODE(hash_table_p,key,nodeobj_p,typecast,offset)\
 struct mchash_table_link* hash_link_p;\
 hash_link_p = (struct mchash_table_link*)((&((hash_table_p)->bucket_list[key]))->head.next_p); \
 if (hash_link_p != NULL) \
   nodeobj_p = rcu_dereference((typecast)((unsigned char*)hash_link_p - offset)); \
 else \
   nodeobj_p = NULL;


#define MCHASH_TABLE_SCAN_FROM_GIVEN_KEY(hash_table_p,hashkey) \
   for ((key = hashkey); (key) < (hash_table_p)->buckets; (key) = (key) + 1) 



#define MCHASH_ACCESS_NODE_AND_HASHKEY(hash_table_p,noderef_index,noderef_magic,nodeobj_p,offset, hashkey,status)\
{\
struct array_indirect_entry* noderef_p = (&(hash_table_p->indirection_array_p[noderef_index]));\
struct mchash_table_link *table_link;\
status = FALSE;\
if(noderef_magic == noderef_p->magic)\
{\
nodeobj_p = rcu_dereference(noderef_p->node_p);\
table_link = (struct mchash_table_link *)((unsigned char*)(noderef_p->node_p) + offset);\
hashkey=table_link->key;\
status = TRUE;\
}\
}

#define MCHASH_INSERT_NODE_AFTER(hash_table_p,node,keys,nodeobj_p,\
mchash_noderef_index,mchash_noderef_magic,hashobj_p,status)\
{\
  struct mchash_bucket* bucket_p;\
  struct array_indirect_entry* array_node_p;\
  cntlr_assert(hash_table_p);\
  cntlr_assert(nodeobj_p); \
  status = FALSE; \
  CNTLR_LOCK_TAKE(hash_table_p->mchash_table_lock); \
  if(hash_table_p->free_q.next_p != NULL) \
  { \
    array_node_p = (struct array_indirect_entry *)hash_table_p->free_q.next_p;\
    hash_table_p->free_q.next_p = array_node_p->next_p;\
    if(hash_table_p->free_q.next_p == NULL) \
	{\
      hash_table_p->free_q.prev_p = NULL; \
	}\
    else \
	{\
      (hash_table_p->free_q.next_p)->prev_p = NULL; \
	}\
    array_node_p->magic = hash_table_p->magic_no; \
    hash_table_p->magic_no++; \
    if(0 == hash_table_p->magic_no) \
	{\
     hash_table_p->magic_no = 1; \
	}\
    hash_table_p->active_nodes++; \
    CNTLR_LOCK_RELEASE(hash_table_p->mchash_table_lock); \
    array_node_p->node_p = nodeobj_p; \
    bucket_p = &((hash_table_p)->bucket_list[(keys)]); \
    mchash_noderef_magic = array_node_p->magic; \
    mchash_noderef_index = array_node_p - hash_table_p->indirection_array_p;\
    ((struct mchash_table_link *)hashobj_p)->index = mchash_noderef_index; \
    CNTLR_LOCK_TAKE(bucket_p->mchash_bucket_lock); \
    mchash_insert_node_in_middle(bucket_p,(struct mchash_dll_node *)&node,(struct mchash_dll_node *)hashobj_p,node.next_p);\
    ((struct mchash_table_link*)hashobj_p)->key = keys;\
    CNTLR_LOCK_RELEASE(bucket_p->mchash_bucket_lock); \
    status = TRUE; \
  } \
  else \
{\
    CNTLR_LOCK_RELEASE(hash_table_p->mchash_table_lock); \
}\
}  

#define MCHASH_INSERT_NODE_BEFORE(hash_table_p,node,keys,nodeobj_p,mchash_noderef_index,mchash_noderef_magic,hashobj_p,status)\
{\
  struct mchash_bucket* bucket_p;\
  struct array_indirect_entry* array_node_p;\
  cntlr_assert(hash_table_p);\
  cntlr_assert(nodeobj_p); \
  status = FALSE; \
  CNTLR_LOCK_TAKE(hash_table_p->mchash_table_lock); \
  if(hash_table_p->free_q.next_p != NULL) \
  { \
  /*Take free Indirection Node, Assign Magic Number & node_p pointer,\
    Increament uiActiveNodes & magic*/ \
    array_node_p = (struct array_indirect_entry *)hash_table_p->free_q.next_p;\
    hash_table_p->free_q.next_p = array_node_p->next_p;\
    if(hash_table_p->free_q.next_p == NULL) \
      hash_table_p->free_q.prev_p = NULL; \
    else \
      (hash_table_p->free_q.next_p)->prev_p = NULL; \
    array_node_p->magic = hash_table_p->magic_no; \
    hash_table_p->magic_no++; \
    if(0 == hash_table_p->magic_no) hash_table_p->magic_no = 1; \
    hash_table_p->active_nodes++; \
    CNTLR_LOCK_RELEASE(hash_table_p->mchash_table_lock); \
    array_node_p->node_p = nodeobj_p; \
    bucket_p = &((hash_table_p)->bucket_list[(keys)]); \
    mchash_noderef_magic = array_node_p->magic; \
    mchash_noderef_index = array_node_p - hash_table_p->indirection_array_p;\
    ((struct mchash_table_link *)hashobj_p)->index = mchash_noderef_index; \
    CNTLR_LOCK_TAKE(bucket_p->mchash_bucket_lock); \
    mchash_insert_node_in_middle(bucket_p,node.prev_p,(struct mchash_dll_node *)hashobj_p,(struct mchash_dll_node *)&node);\
    ((struct mchash_table_link*)hashobj_p)->key = keys;\
    CNTLR_LOCK_RELEASE(bucket_p->mchash_bucket_lock); \
    status = TRUE; \
  } \
  else \
    CNTLR_LOCK_RELEASE(hash_table_p->mchash_table_lock); \
} 

#endif /*__MC_HASH_H*/
