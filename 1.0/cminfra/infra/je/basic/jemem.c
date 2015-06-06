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
 * File name: jemem.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description:
*/

#ifdef CM_SUPPORT
#ifdef CM_JE_SUPPORT
#include "jeincld.h"
#include "cmsock.h"
#include "jeldef.h"
#include "jemem.h"

/******************************************************************************
 * * * * * * * * * * * * * * * GLOBAL VARIABLES* * * * * * * * * * * * * * * * 
 *****************************************************************************/
void *cfg_session_mempool_p = NULL;
void *cfg_cache_mempool_p = NULL;
void *cfg_request_mempool_p = NULL;
void *tnsprt_channel_mempool_p = NULL;

/******************************************************************************
 * * * * * * * * * * * * * FUNCTION DEFINITIONS * * * * * * * * * * * * * * * * 
 *****************************************************************************/
/**
 \ingroup JEAPI
  This APIcreates Memory Pool to create and store configuration sessions.This will
   call cm_je_get_cfgsession_cnf() to get the   MAXIMUM number of configuration sessions
   that can be created and will allocate Pool for that much number of Sessions
 
  <b>Input paramameters: </b> NONE\n
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> NONE\n
 */
/******************************************************************************
 * Function Name : cm_je_create_cfgsession_mempool
 * Description   : This API  creates Memory Pool to create and store configuration
 *                 sessions.  This will call cm_je_get_cfgsession_cnf() to get the
 *                 MAXIMUM number of configuration sessions that can be created and
 *                 will allocate Pool for that much number of Sessions.
 * Input params  : None
 * Output params : None
 * Return value  : None
 *****************************************************************************/
void cm_je_create_cfgsession_mempool (void)
{
  uint32_t cfg_session_tables;
  uint32_t cfg_session_tables_static;
  uint32_t cfg_session_threshold_max;
  uint32_t cfg_session_threshold_min;
  uint32_t cfg_session_replinish;
  int32_t return_value;

  cm_je_get_cfgsession_cnf (&cfg_session_tables,
                         &cfg_session_tables_static,
                         &cfg_session_threshold_max,
                         &cfg_session_threshold_min,
                         &cfg_session_replinish);

  /* static memory pool for Configuration Session  Nodes */
  if (cfg_session_tables_static > 0)
  {
    cfg_session_mempool_p =
      cm_create_mempool (cfg_session_tables_static,
                        sizeof (struct je_config_session),
                        cfg_session_tables, TRUE);
    if (cfg_session_mempool_p == NULL)
    {
      fprintf (stderr,
                 "cm_je_create_cfgsession_mempool :: cm_create_mempool failed\r\n");
      return;                   /* Exit the task that is calling this function */
    }
    return_value = cm_mempool_set_threshold_vals (cfg_session_mempool_p,
                                          cfg_session_threshold_min,
                                          cfg_session_threshold_max,
                                          cfg_session_replinish);
    if (return_value != OF_SUCCESS)
    {
      CM_JE_DEBUG_PRINT ("cm_mempool_set_threshold_vals failed");
      cm_delete_mempool (cfg_session_mempool_p);
      return;
    }
  }

  return;
}

/**
 \ingroup JEAPI
  This function is used to allocate memory block for a Configuration Session node
   from preallocated memory pool. If no buffers  exists in the Pool will allocate
   memory from Heap.
 
  <b>Input paramameters: </b> NONE\n
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> void pointer to allocated memory block or NULL\n
 */
/****************************************************************************
 * FunctionName  : cm_je_cfgsession_alloc
 * Description   : This function is used to allocate memory block for a Configuration
 *                  Session node from preallocated memory pool. If no buffers 
 *                 exists in the Pool will allocate memory from Heap.
 * Input         : NONE
 * Output        : NONE
 * Return value  : on success retuens Allocate Memory block address
 *                 NULL - on failure
 ***************************************************************************/
void *cm_je_cfgsession_alloc (void)
{
  struct je_config_session *cfg_session_p = NULL;
  unsigned char heap_b = TRUE;
  int16_t return_value;

  /* Always we will set the heap_b value to TRUE.
   * While calling cm_mempool_get_block() , this heap_b will act as Input and
   * Output parameter. If memory block is allocated from the Pool, it's value
   * will be set to FALSE. Otherwise, if memory is exhausted in thr Pool,
   * based on heap_b value(TRUE) memory will be allocated from Heap.*/

  /* Memory allocation failed */
  return_value = cm_mempool_get_block (cfg_session_mempool_p,
                                (unsigned char **) & cfg_session_p, &heap_b);

  /* Memory allocation failed */
  if (cfg_session_p == NULL || return_value != OF_SUCCESS)
  {
    CM_JE_DEBUG_PRINT ("cm_mempool_get_block Failed for policy");
    return (NULL);
  }
  cfg_session_p->heap_b = heap_b;
  return (cfg_session_p);
}

/**
 \ingroup JEAPI
  This function is used to release memory block allocated for a Configuration 
  Session node. Here bFlag of Configuration Session Node specifies whether the memory 
  for the block is  allocated from Preallocated memory Pool (OR) from  Heap
 
  <b>Input paramameters: </b>\n
  <b><i>pFree</i></b> points to the memory block
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> NONE \n
 */
/****************************************************************************
 * FunctionName  : cm_je_cfgsession_free
 * Description   : Used to release the memory block allocated for a Configuration Session
 *                  node.
 * Input         : pFree -- points to the memory block
 *                 heap_b -- specifies whether the memory for the block is
 *                          allocated from Preallocated memory Pool (OR) from
 *                          Heap.
 * Output        : NONE
 * Return value  : NONE
 ***************************************************************************/
void cm_je_cfgsession_free (struct je_config_session * pFree)
{
  if (cm_mempool_release_block (cfg_session_mempool_p, (unsigned char *) pFree,
                              pFree->heap_b) != OF_SUCCESS)
  {
    CM_JE_DEBUG_PRINT
      (" cm_mempool_release_block Failed for Configuration Session node");
    return;
  }

  return;
}

/**
 \ingroup JEAPI
  This APIcreates Memory Pool to create and store configuration Caches.This will
   call cm_je_get_cfgcache_cnf() to get the   MAXIMUM number of configuration Caches
   that can be created and will allocate Pool for that much number of Caches
 
  <b>Input paramameters: </b> NONE\n
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> NONE\n
 */
/******************************************************************************
 * Function Name : cm_je_create_cfgcache_mempool
 * Description   : This API  creates Memory Pool to create and store configuration
 *                 Caches.  This will call cm_je_get_cfgcache_cnf() to get the
 *                 MAXIMUM number of configuration Caches that can be created and
 *                 will allocate Pool for that much number of Caches.
 * Input params  : None
 * Output params : None
 * Return value  : None
 *****************************************************************************/
void cm_je_create_cfgcache_mempool (void)
{
  uint32_t cfg_cache_tables;
  uint32_t cfg_cache_tables_static;
  uint32_t cfg_cache_threshold_max;
  uint32_t cfg_cache_threshold_min;
  uint32_t cfg_cache_replinish;
  int32_t return_value;

  cm_je_get_cfgcache_cnf (&cfg_cache_tables,
                       &cfg_cache_tables_static,
                       &cfg_cache_threshold_max,
                       &cfg_cache_threshold_min, &cfg_cache_replinish);

  /* static memory pool for Configuration Cache  Nodes */
  if (cfg_cache_tables_static > 0)
  {
    cfg_cache_mempool_p =
      cm_create_mempool (cfg_cache_tables_static,
                        sizeof (struct je_config_session_cache),
                        cfg_cache_tables, TRUE);
    if (cfg_cache_mempool_p == NULL)
    {
      fprintf (stderr,
                 "cm_je_create_cfgcache_mempool :: cm_create_mempool failed\r\n");
      return;                   /* Exit the task that is calling this function */
    }
    return_value = cm_mempool_set_threshold_vals (cfg_cache_mempool_p,
                                          cfg_cache_threshold_min,
                                          cfg_cache_threshold_max,
                                          cfg_cache_replinish);
    if (return_value != OF_SUCCESS)
    {
      fprintf (stderr,
                 "%s()::cm_mempool_set_threshold_vals failed\r\n", __FUNCTION__);
      cm_delete_mempool (cfg_cache_mempool_p);
      return;
    }
  }

  return;
}

/**
 \ingroup JEAPI
  This function is used to allocate memory block for a Configuration Cache node
   from preallocated memory pool. If no buffers  exists in the Pool will allocate
   memory from Heap.
 
  <b>Input paramameters: </b> NONE\n
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> void pointer to allocated memory block or NULL\n
 */
/****************************************************************************
 * FunctionName  : cm_je_cfgcache_alloc
 * Description   : This function is used to allocate memory block for a Configuration
 *                  Cache node from preallocated memory pool. If no buffers 
 *                 exists in the Pool will allocate memory from Heap.
 * Input         : NONE
 * Output        : NONE
 * Return value  : on success retuens Allocate Memory block address
 *                 NULL - on failure
 ***************************************************************************/
void *cm_je_cfgcache_alloc (void)
{
  struct je_config_session_cache *cfg_cache_p = NULL;
  unsigned char heap_b = TRUE;
  int16_t return_value;

  /* Always we will set the heap_b value to TRUE.
   * While calling cm_mempool_get_block() , this heap_b will act as Input and
   * Output parameter. If memory block is allocated from the Pool, it's value
   * will be set to FALSE. Otherwise, if memory is exhausted in thr Pool,
   * based on heap_b value(TRUE) memory will be allocated from Heap.*/

  /* Memory allocation failed */
  return_value = cm_mempool_get_block (cfg_cache_mempool_p,
                                (unsigned char **) & cfg_cache_p, &heap_b);

  /* Memory allocation failed */
  if (cfg_cache_p == NULL || return_value != OF_SUCCESS)
  {
    CM_JE_DEBUG_PRINT ("cm_mempool_get_block Failed for policy");
    return (NULL);
  }
  cfg_cache_p->heap_b = heap_b;
  return (cfg_cache_p);
}

/**
 \ingroup JEAPI
  This function is used to release memory block allocated for a Configuration 
  Cache node. Here bFlag of Configuration Cache Node specifies whether the memory 
  for the block is  allocated from Preallocated memory Pool (OR) from  Heap
 
  <b>Input paramameters: </b>\n
  <b><i>pFree</i></b> points to the memory block
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> NONE \n
 */
/****************************************************************************
 * FunctionName  : cm_je_cfgcache_free
 * Description   : Used to release the memory block allocated for a Configuration Cache
 *                  node.
 * Input         : pFree -- points to the memory block
 *                 heap_b -- specifies whether the memory for the block is
 *                          allocated from Preallocated memory Pool (OR) from
 *                          Heap.
 * Output        : NONE
 * Return value  : NONE
 ***************************************************************************/
void cm_je_cfgcache_free (struct je_config_session_cache * pFree)
{
  if (cm_mempool_release_block (cfg_cache_mempool_p, (unsigned char *) pFree,
                              pFree->heap_b) != OF_SUCCESS)
  {
    CM_JE_DEBUG_PRINT
      ("cm_mempool_release_block Failed for Configuration Cache node");
    return;
  }

  return;
}

/**
 \ingroup JEAPI
  This APIcreates Memory Pool to create and store configuration Requests.This will
   call cm_je_get_cfgrequest_cnf() to get the   MAXIMUM number of configuration Requests
   that can be created and will allocate Pool for that much number of Requests
 
  <b>Input paramameters: </b> NONE\n
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> NONE\n
 */
/******************************************************************************
 * Function Name : cm_je_create_cfgrequest_mempool
 * Description   : This API  creates Memory Pool to create and store configuration
 *                 Requests.  This will call cm_je_get_cfgrequest_cnf() to get the
 *                 MAXIMUM number of configuration Requests that can be created and
 *                 will allocate Pool for that much number of Requests.
 * Input params  : None
 * Output params : None
 * Return value  : None
 *****************************************************************************/

void cm_je_create_cfgrequest_mempool (void)
{
  uint32_t cfg_request_tables;
  uint32_t cfg_request_tables_static;
  uint32_t cfg_request_threshold_max;
  uint32_t cfg_request_threshold_min;
  uint32_t cfg_request_replinish;
  int32_t return_value;

  cm_je_get_cfgrequest_cnf (&cfg_request_tables,
                         &cfg_request_tables_static,
                         &cfg_request_threshold_max,
                         &cfg_request_threshold_min,
                         &cfg_request_replinish);

  /* static memory pool for Configuration Cache  Nodes */
  if (cfg_request_tables_static > 0)
  {
    cfg_request_mempool_p =
      cm_create_mempool (cfg_request_tables_static,
                        sizeof (struct je_config_request),
                        cfg_request_tables, TRUE);
    if (cfg_request_mempool_p == NULL)
    {
      fprintf (stderr,
                 "cm_je_create_cfgrequest_mempool :: cm_create_mempool failed\r\n");
      return;                   /* Exit the task that is calling this function */
    }
    return_value = cm_mempool_set_threshold_vals (cfg_request_mempool_p,
                                          cfg_request_threshold_min,
                                          cfg_request_threshold_max,
                                          cfg_request_replinish);
    if (return_value != OF_SUCCESS)
    {
      fprintf (stderr, "%s()::cm_mempool_set_threshold_vals failed\r\n", __FUNCTION__);
      cm_delete_mempool (cfg_request_mempool_p);
      return;
    }
  }

  return;
}

/**
 \ingroup JEAPI
  This function is used to allocate memory block for a Configuration Request node
   from preallocated memory pool. If no buffers  exists in the Pool will allocate
   memory from Heap.
 
  <b>Input paramameters: </b> NONE\n
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> void pointer to allocated memory block or NULL\n
 */
/****************************************************************************
 * FunctionName  : cm_je_cfgrequest_alloc
 * Description   : This function is used to allocate memory block for a Configuration
 *                  Request node from preallocated memory pool. If no buffers 
 *                 exists in the Pool will allocate memory from Heap.
 * Input         : NONE
 * Output        : NONE
 * Return value  : on success retuens Allocate Memory block address
 *                 NULL - on failure
 ***************************************************************************/

void *cm_je_cfgrequest_alloc (void)
{
  struct je_config_request *cfg_request_p = NULL;
  unsigned char heap_b = TRUE;
  int16_t return_value;

  /* Always we will set the heap_b value to TRUE.
   * While calling cm_mempool_get_block() , this heap_b will act as Input and
   * Output parameter. If memory block is allocated from the Pool, it's value
   * will be set to FALSE. Otherwise, if memory is exhausted in thr Pool,
   * based on heap_b value(TRUE) memory will be allocated from Heap.*/

  /* Memory allocation failed */
  return_value = cm_mempool_get_block (cfg_request_mempool_p,
                                (unsigned char **) & cfg_request_p, &heap_b);

  /* Memory allocation failed */
  if (cfg_request_p == NULL || return_value != OF_SUCCESS)
  {
    CM_JE_DEBUG_PRINT ("cm_mempool_get_block Failed for policy");
    return (NULL);
  }
  cfg_request_p->heap_b = heap_b;
  return (cfg_request_p);
}

/**
 \ingroup JEAPI
  This function is used to release memory block allocated for a Configuration 
  Request node. Here bFlag of Configuration Request Node specifies whether the memory 
  for the block is  allocated from Preallocated memory Pool (OR) from  Heap
 
  <b>Input paramameters: </b>\n
  <b><i>pFree</i></b> points to the memory block
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> NONE \n
 */
/****************************************************************************
 * FunctionName  : cm_je_cfgrequest_free
 * Description   : Used to release the memory block allocated for a Configuration Request
 *                  node.
 * Input         : pFree -- points to the memory block
 *                 heap_b -- specifies whether the memory for the block is
 *                          allocated from Preallocated memory Pool (OR) from
 *                          Heap.
 * Output        : NONE
 * Return value  : NONE
 ***************************************************************************/
void cm_je_cfgrequest_free (struct je_config_request * pFree)
{
  if (cm_mempool_release_block (cfg_request_mempool_p, (unsigned char *) pFree,
                              pFree->heap_b) != OF_SUCCESS)
  {
    CM_JE_DEBUG_PRINT
      ("cm_mempool_release_block Failed for Configuration Cache node");
    return;
  }

  return;
}

/****************************************************************************
 * * *  * * * * * * * * TRANSPORT CHANNEL * * * * * * * * * * * * * * * * * * 
 ****************************************************************************/

/**
 \ingroup JEAPI
  This APIcreates Memory Pool to create and store Transport Channels.This will
   call cm_je_get_tnsprtchannel_cnf() to get the   MAXIMUM number of Transport Channels
   that can be created and will allocate Pool for that much number of Sessions
 
  <b>Input paramameters: </b> NONE\n
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> NONE\n
 */
/******************************************************************************
 * Function Name : cm_je_create_tnsprtchannel_mempool
 * Description   : This API  creates Memory Pool to create and store Transport
 *                 Channels.  This will call cm_je_get_tnsprtchannel_cnf() to get the
 *                 MAXIMUM number of Transport Channels that can be created and
 *                 will allocate Pool for that much number of Sessions.
 * Input params  : None
 * Output params : None
 * Return value  : None
 *****************************************************************************/
void cm_je_create_tnsprtchannel_mempool (void)
{
  uint32_t tnsprt_channel_tables;
  uint32_t tnsprt_channel_tables_static;
  uint32_t tnsprt_channel_threshold_max;
  uint32_t tnsprt_channel_threshold_min;
  uint32_t tnsprt_channel_replinish;
  int32_t return_value;

  cm_je_get_tnsprtchannel_cnf (&tnsprt_channel_tables,
                               &tnsprt_channel_tables_static,
                               &tnsprt_channel_threshold_max,
                               &tnsprt_channel_threshold_min,
                               &tnsprt_channel_replinish);

  /* static memory pool for Transport channel Nodes */
  if (tnsprt_channel_tables_static > 0)
  {
    tnsprt_channel_mempool_p =
      cm_create_mempool (tnsprt_channel_tables_static,
                        sizeof (struct cmje_tnsprtchannel_info),
                        tnsprt_channel_tables, TRUE);
    if (tnsprt_channel_mempool_p == NULL)
    {
      fprintf (stderr,
                 "cm_je_create_tnsprtchannel_mempool :: cm_create_mempool failed");
      return;                   /* Exit the task that is calling this function */
    }
    return_value = cm_mempool_set_threshold_vals (tnsprt_channel_mempool_p,
                                          tnsprt_channel_threshold_min,
                                          tnsprt_channel_threshold_max,
                                          tnsprt_channel_replinish);
    if (return_value != OF_SUCCESS)
    {
      fprintf (stderr, "cm_mempool_set_threshold_vals failed");
      cm_delete_mempool (tnsprt_channel_mempool_p);
      return;
    }
  }

  return;
}

/**
 \ingroup JEAPI
  This function is used to allocate memory block for a Transport Channel Node
   from preallocated memory pool. If no buffers  exists in the Pool will allocate
   memory from Heap.
 
  <b>Input paramameters: </b> NONE\n
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> void pointer to allocated memory block or NULL\n
 */
/****************************************************************************
 * FunctionName  : cm_je_tnsprtchannel_alloc
 * Description   : This function is used to allocate memory block for a Transport 
 *                 Channel node from preallocated memory pool. If no buffers 
 *                 exists in the Pool will allocate memory from Heap.
 * Input         : NONE
 * Output        : NONE
 * Return value  : on success retuens Allocate Memory block address
 *                 NULL - on failure
 ***************************************************************************/
void *cm_je_tnsprtchannel_alloc (void)
{
  struct cmje_tnsprtchannel_info *tnsprt_channel_p = NULL;
  unsigned char heap_b = TRUE;
  int16_t return_value;

  /* Always we will set the heap_b value to TRUE.
   * While calling cm_mempool_get_block() , this heap_b will act as Input and
   * Output parameter. If memory block is allocated from the Pool, it's value
   * will be set to FALSE. Otherwise, if memory is exhausted in thr Pool,
   * based on heap_b value(TRUE) memory will be allocated from Heap.*/
  return_value = cm_mempool_get_block (tnsprt_channel_mempool_p,
                                (unsigned char **) & tnsprt_channel_p, &heap_b);

  /* Memory allocation failed */
  if (tnsprt_channel_p == NULL || return_value != OF_SUCCESS)
  {
    CM_JE_DEBUG_PRINT ("cm_mempool_get_block Failed for policy");
    return (NULL);
  }
  tnsprt_channel_p->heap_b = heap_b;
  return (tnsprt_channel_p);
}

/**
 \ingroup JEAPI
  This function is used to release memory block allocated for a Transport
  Channel node. Here bFlag of Configuration Session Node specifies whether the memory 
  for the block is  allocated from Preallocated memory Pool (OR) from  Heap
 
  <b>Input paramameters: </b>\n
  <b><i>pFree</i></b> points to the memory block
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> NONE \n
 */
/****************************************************************************
 * FunctionName  : cm_je_tnsprtchannel_free
 * Description   : Used to release the memory block allocated for a Transport
 *                 Channel node.
 * Input         : pFree -- points to the memory block
 *                 heap_b -- specifies whether the memory for the block is
 *                          allocated from Preallocated memory Pool (OR) from
 *                          Heap.
 * Output        : NONE
 * Return value  : NONE
 ***************************************************************************/
void cm_je_tnsprtchannel_free (struct cmje_tnsprtchannel_info * pFree)
{
  if (cm_mempool_release_block (tnsprt_channel_mempool_p, (unsigned char *) pFree,
                              pFree->heap_b) != OF_SUCCESS)
  {
    CM_JE_DEBUG_PRINT (" cm_mempool_release_block Failed for Transport \
                                 Channel node");
    return;
  }
  return;
}

#endif /* CM_JE_SUPPORT */
#endif /* CM_SUPPORT */
