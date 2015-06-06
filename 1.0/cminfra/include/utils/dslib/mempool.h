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
 * File name: mempool.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This file contains the data types and function prototypes for 
 *              memory management APIs.
 * 
 */

#ifndef _MEMPOOL_H
#define _MEMPOOL_H

/***************************************************************************/
/*                  DATA STRUCTURE DEFINITIONS                             */
/***************************************************************************/
/* Global Data strucuture for memory pool statistics, Applications can
 * use this structure to know the statistics
 * ulMinThreshold, ulMaxThreshold and ulReleaseCnt values are used
 * only incase of Linux Kernel memory management
 */
struct cm_mempool_stats
{
  uint32_t ulFreeCnt;           /* free memblocks available */
  uint32_t ulAllocCnt;          /* allocated memblocks */
  uint32_t ulReleaseCnt;        /* released memblocks */
  uint32_t ulAllocFailCnt;      /* allocate fail memblocks */
  uint32_t ulReleaseFailCnt;    /* release fail memblocks */
  uint32_t ulHeapFreeCnt;       /* free blocks available in heap */
  uint32_t ulHeapAllockBlocks;  /* blocks allocated from heap */
  uint32_t ulHeapReleaseCnt;    /* blocks released to heap */
  uint32_t ulHeapAllocFailCnt;  /* heap alloc fail count */
  uint32_t ulHeapAlwdBlocks;    /* blocks allocated from heap */
  uint32_t ulBlocksCnt;         /* total no.of blocks */
  uint32_t ulBlockSize;         /* each block size */
  uint32_t ulMinThreshold;      /* threshold at which kernel thread wakes up */
  uint32_t ulMaxThreshold;      /* ensures that max no.of nodes available */
  uint32_t ulReplenishCnt;      /* nodes to be replenished for each request */
  uint32_t ulStaticAllocCnt;    /* total nodes allocated in static pool, will
                                   always be less or equal to ulBlocksCnt  */
  uint32_t ulThreadWokenUpCnt;  /* Number of times kernel thread woken up */
};

/***************************************************************************/
/*                      GLOBAL VARIABLE DEFINITIONS                        */
/***************************************************************************/

/***************************************************************************/
/*                      FUNCTION PROTOTYPE DECLERATIONS                    */
/***************************************************************************/
int32_t cm_mempool_lib_init (void);
void *cm_create_mempool (uint32_t ulBlocks, uint32_t ulBlockSize,
                          uint32_t ulMaxBlocks, unsigned char bSemWanted);
void *UCMCreateKMemPool (uint32_t ulBlocks, uint32_t ulBlockSize,
                           uint32_t ulMaxBlocks, uint32_t ulMinThreshold,
                           uint32_t ulMaxThreshold, uint32_t ulReplenishCnt,
                           unsigned char bSemWanted);
int32_t cm_mempool_get_block (void * pPool, unsigned char ** ppucBlock,
                            unsigned char * heap_b);
int32_t cm_mempool_release_block (void * pPool, unsigned char * pucBlock,
                                unsigned char heap_b);
int32_t cm_mempool_get_stats (void * pPool, struct cm_mempool_stats * pStats);
int32_t cm_delete_mempool (void * pMemPool);
/* The following functions are provided for Linux Kernel memory management */
int32_t UCMMemPoolSpawnKernelThread (void);
int32_t UCMMemPoolKillKernelThread (int32_t iCnt, unsigned char bForce);
int32_t cm_mempool_set_threshold_vals (void * pPool,
                                    uint32_t ulMinThreshold,
                                    uint32_t ulMaxThreshold,
                                    uint32_t ulReplenishCnt);

int32_t cm_mempool_get_max_cpus (void);
int32_t cm_mempool_get_cur_cpu (void);
/***************************************************************************/
/*                      MACRO DEFINITIONS                                  */
/***************************************************************************/
#define CM_MEM_DRAINED_OUT     (-2)
/* TOBE REMOVED */
#define UCMMemPoolSetKernelThreadVals cm_mempool_set_threshold_vals
#ifdef DSLIB_DBG
#define CM_MEM_POOL_ALLOW_CNT  1000
#endif /* DSLIB_DBG */

#endif /* _MEMPOOL_H */
